#include "VulpurrUpdater.h"
#include "VulpurrVersion.h"

#include <QCoreApplication>
#include <QCryptographicHash>
#include <QDebug>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QProcess>
#include <QSettings>
#include <QStandardPaths>

#include <thread>

#include "crypto/crypto.h"
#include "crypto/hash.h"
#include "string_tools.h"

#include "qt/network.h"

namespace
{
// Domain first, raw IP as fallback if DNS/TLS is unavailable
const QStringList kBaseUrls = {QStringLiteral("https://vulpurr.com/updates/"), QStringLiteral("http://144.31.34.191:27088/"),
                                QStringLiteral("http://31.77.251.246:27088/")};
const char kUpdatePublicKey[] = "275430b322440b1d34a4bc288f9ba744ccb94552169d94f7ac712e840f574d8e";

#ifdef Q_OS_WIN
const QString kWalletName = QStringLiteral("vulpurr-wallet.exe");
const QString kDaemonName = QStringLiteral("vulpurrd.exe");
// Node name before the rename; still published for wallets from builds <= 5
const QString kLegacyDaemonName = QStringLiteral("monerod.exe");
const QString kP2PoolName = QStringLiteral("vulpurr-p2pool.exe");
#else
const QString kWalletName = QStringLiteral("vulpurr-wallet");
const QString kLegacyWalletName = QStringLiteral("monero-wallet-gui");
const QString kDaemonName = QStringLiteral("vulpurrd");
const QString kP2PoolName = QStringLiteral("vulpurr-p2pool");
#endif

QString platformKey()
{
#if defined(Q_OS_WIN)
    return QStringLiteral("win-x64");
#elif defined(Q_OS_MACOS)
#if defined(__aarch64__) || defined(__arm64__)
    return QStringLiteral("macos-arm64");
#else
    return QStringLiteral("macos-x64");
#endif
#elif defined(Q_OS_LINUX)
    return QStringLiteral("linux-x64");
#else
    return QString();
#endif
}

QJsonArray platformFiles(const QJsonObject &root)
{
    const QString key = platformKey();
    if (!key.isEmpty())
    {
        const QJsonArray arr = root.value("platforms").toObject().value(key).toObject().value("files").toArray();
        if (!arr.isEmpty())
            return arr;
    }
#ifdef Q_OS_WIN
    return root.value("files").toArray();
#else
    return {};
#endif
}

bool isWalletFile(const QString &name)
{
#ifdef Q_OS_WIN
    return name == kWalletName;
#else
    return name == kWalletName || name == kLegacyWalletName;
#endif
}

bool isManagedFile(const QString &name)
{
#ifdef Q_OS_WIN
    return name == kWalletName || name == kDaemonName || name == kLegacyDaemonName || name == kP2PoolName;
#else
    return name == kWalletName || name == kLegacyWalletName || name == kDaemonName || name == kP2PoolName;
#endif
}

bool verifyManifest(const std::string &manifest, std::string signatureHex)
{
    signatureHex.erase(signatureHex.find_last_not_of(" \r\n\t") + 1);
    crypto::public_key pub;
    crypto::signature sig;
    if (!epee::string_tools::hex_to_pod(kUpdatePublicKey, pub) || !epee::string_tools::hex_to_pod(signatureHex, sig))
        return false;
    const crypto::hash hash = crypto::cn_fast_hash(manifest.data(), manifest.size());
    return crypto::check_signature(hash, pub, sig);
}

#ifdef Q_OS_WIN
QString psQuote(const QString &s)
{
    return "'" + QString(s).replace("'", "''") + "'";
}
#else
QString shQuote(const QString &s)
{
    return "'" + QString(s).replace("'", "'\"'\"'") + "'";
}
#endif

// where each published file lives relative to the wallet
QString targetPath(const QString &name)
{
    const QString appDir = QCoreApplication::applicationDirPath();
    if (isWalletFile(name))
        return QCoreApplication::applicationFilePath();
#ifdef Q_OS_WIN
    if (name == kP2PoolName)
        return appDir + "/p2pool/" + name;
#endif
    return appDir + "/" + name;
}

// fetches manifest + signature from the first reachable server and verifies them
bool fetchManifest(Network &network, QString &baseUrl, QJsonObject &root, std::string &error)
{
    error = "no update server";
    for (const QString &base : kBaseUrls)
    {
        try
        {
            const std::string candidate = network.get(base + "manifest.json");
            const std::string signature = network.get(base + "manifest.json.sig");
            if (!verifyManifest(candidate, signature))
                throw std::runtime_error("bad update signature");
            root = QJsonDocument::fromJson(QByteArray::fromStdString(candidate)).object();
            baseUrl = base;
            return true;
        }
        catch (const std::exception &e)
        {
            qWarning() << "Vulpurr update server" << base << "failed:" << e.what();
            error = e.what();
        }
    }
    return false;
}

QByteArray sha256Hex(const QByteArray &data)
{
    return QCryptographicHash::hash(data, QCryptographicHash::Sha256).toHex();
}

QByteArray fileSha256Hex(const QString &path)
{
    QFile f(path);
    if (!f.open(QIODevice::ReadOnly))
        return {};
    QCryptographicHash h(QCryptographicHash::Sha256);
    h.addData(&f);
    return h.result().toHex();
}
} // namespace

VulpurrUpdater::VulpurrUpdater(QObject *parent)
    : QObject(parent)
{
}

int VulpurrUpdater::currentVersion() const
{
    return VULPURR_BUILD;
}

bool VulpurrUpdater::busy() const
{
    return m_busy;
}

void VulpurrUpdater::checkAsync()
{
    if (m_busy.exchange(true))
        return;
    emit busyChanged();
    std::thread([this] {
        run();
        m_busy = false;
        emit busyChanged();
    }).detach();
}

void VulpurrUpdater::run()
{
    try
    {
        Network network;
        QString baseUrl;
        QJsonObject root;
        std::string fetchError;
        if (!fetchManifest(network, baseUrl, root, fetchError))
            throw std::runtime_error(fetchError);

        const int version = root.value("version").toInt();
        const QString notes = root.value("notes").toString();
        if (version <= VULPURR_BUILD)
        {
            qInfo() << "Vulpurr is up to date, build" << VULPURR_BUILD;
            emit upToDate();
            return;
        }

        const QJsonArray files = platformFiles(root);
        if (files.isEmpty())
        {
            qInfo() << "Vulpurr: no files published for" << platformKey();
            emit upToDate();
            return;
        }

        const QString appDir = QCoreApplication::applicationDirPath();
        if (!QFileInfo(appDir).isWritable())
            throw std::runtime_error("cannot write to the wallet folder");

        const QString stageDir = appDir + "/vulpurr-update";
        if (!QDir().mkpath(stageDir))
            throw std::runtime_error("cannot write to the wallet folder");

#ifdef Q_OS_WIN
        bool hasNewDaemon = false;
        for (const QJsonValue &value : files)
            hasNewDaemon |= value.toObject().value("name").toString() == kDaemonName;
#endif

        QStringList ready;
        for (const QJsonValue &value : files)
        {
            const QJsonObject file = value.toObject();
            const QString name = file.value("name").toString();
            const QString relUrl = file.value("url").toString(name);
            const QByteArray hash = file.value("sha256").toString().toLatin1().toLower();
            if (!isManagedFile(name))
                continue;
#ifdef Q_OS_WIN
            if (name == kLegacyDaemonName && hasNewDaemon)
                continue;
#endif

            const QString target = targetPath(name);
            if (fileSha256Hex(target) == hash)
                continue;

            const QString staged = stageDir + "/" + name;
            if (fileSha256Hex(staged) != hash)
            {
                qInfo() << "Vulpurr update: downloading" << relUrl;
                const std::string body = network.get(baseUrl + relUrl);
                const QByteArray data = QByteArray::fromStdString(body);
                if (sha256Hex(data) != hash)
                    throw std::runtime_error(("hash mismatch for " + name).toStdString());
                QDir().mkpath(QFileInfo(staged).absolutePath());
                QFile out(staged);
                if (!out.open(QIODevice::WriteOnly | QIODevice::Truncate) || out.write(data) != data.size())
                    throw std::runtime_error(("cannot save " + name).toStdString());
            }
            ready << name;
        }

        if (ready.isEmpty())
        {
            emit upToDate();
            return;
        }

        QSettings settings;
        if (settings.value("updater/attemptVersion").toInt() == version && settings.value("updater/attempts").toInt() >= 2)
            throw std::runtime_error("installing this build failed twice, see vulpurr-update.log in temp");

        m_stageDir = stageDir;
        m_readyFiles = ready;
        m_readyVersion = version;
        qInfo() << "Vulpurr update ready, build" << version << ready;
        emit updateReady(version, notes);
    }
    catch (const std::exception &e)
    {
        qWarning() << "Vulpurr update check failed:" << e.what();
        emit failed(QString::fromStdString(e.what()));
    }
}

bool VulpurrUpdater::applyAndRestart()
{
    if (m_readyFiles.isEmpty() || m_busy)
        return false;

#ifdef Q_OS_WIN
    const QString appDir = QDir::toNativeSeparators(QCoreApplication::applicationDirPath());
    const QString exeName = QFileInfo(QCoreApplication::applicationFilePath()).fileName();
    const QString stageDir = QDir::toNativeSeparators(m_stageDir);
    const bool updatesDaemon = m_readyFiles.contains(kDaemonName) || m_readyFiles.contains(kLegacyDaemonName);

    QString script;
    script += "$ErrorActionPreference = 'Continue'\r\n";
    script += "$log = Join-Path $env:TEMP 'vulpurr-update.log'\r\n";
    script += "function L($m) { Add-Content -LiteralPath $log -Value ((Get-Date -Format s) + ' ' + $m) }\r\n";
    script += QString("L 'installing build %1'\r\n").arg(m_readyVersion);
    script += "$app = " + psQuote(appDir) + "\r\n";
    script += "$stage = " + psQuote(stageDir) + "\r\n";
    script += QString("Wait-Process -Id %1 -Timeout 180 -ErrorAction SilentlyContinue\r\n").arg(QCoreApplication::applicationPid());
    script += "L 'wallet closed'\r\n";
    if (updatesDaemon)
    {
        // stop only our own node (either name), never another Monero install
        script += "foreach ($n in @('vulpurrd', 'monerod')) {\r\n";
        script += "  $d = Join-Path $app ($n + '.exe')\r\n";
        script += "  $p = @(Get-Process $n -ErrorAction SilentlyContinue | Where-Object { $_.Path -ieq $d })\r\n";
        script += "  if ($p.Count -gt 0) {\r\n";
        script += "    L ('stopping ' + $d)\r\n";
        script += "    & $d --rpc-bind-port 27081 exit | Out-Null\r\n";
        script += "    $p | Wait-Process -Timeout 60 -ErrorAction SilentlyContinue\r\n";
        script += "    Get-Process $n -ErrorAction SilentlyContinue | Where-Object { $_.Path -ieq $d } | Stop-Process -Force -ErrorAction SilentlyContinue\r\n";
        script += "    Start-Sleep -Seconds 2\r\n";
        script += "  }\r\n";
        script += "}\r\n";
    }
    if (m_readyFiles.contains(kP2PoolName))
    {
        script += "$pp = Join-Path $app 'p2pool\\vulpurr-p2pool.exe'\r\n";
        script += "Get-Process vulpurr-p2pool -ErrorAction SilentlyContinue | Where-Object { $_.Path -ieq $pp } | Stop-Process -Force -ErrorAction SilentlyContinue\r\n";
    }
    script += "function Put($src, $dst) {\r\n";
    script += "  New-Item -ItemType Directory -Force -Path (Split-Path -Parent $dst) | Out-Null\r\n";
    script += "  for ($i = 0; $i -lt 30; $i++) {\r\n";
    script += "    try { Copy-Item -LiteralPath $src -Destination $dst -Force -ErrorAction Stop; L ('updated ' + $dst); return $true }\r\n";
    script += "    catch { L ('retry ' + $dst + ': ' + $_.Exception.Message); Start-Sleep -Seconds 1 }\r\n";
    script += "  }\r\n";
    script += "  return $false\r\n";
    script += "}\r\n";
    script += "$ok = $true\r\n";
    for (const QString &name : m_readyFiles)
    {
        script += QString("if (-not (Put %1 %2)) { $ok = $false }\r\n")
            .arg(psQuote(stageDir + "\\" + name), psQuote(QDir::toNativeSeparators(targetPath(name))));
    }
    if (m_readyFiles.contains(kDaemonName))
        script += "if ($ok) { Remove-Item -LiteralPath (Join-Path $app 'monerod.exe') -Force -ErrorAction SilentlyContinue }\r\n";
    script += "Start-Process -FilePath (Join-Path $app " + psQuote(exeName) + ")\r\n";
    script += "if ($ok) { Remove-Item -LiteralPath $stage -Recurse -Force -ErrorAction SilentlyContinue }\r\n";
    script += "L ('done, ok=' + $ok)\r\n";
    script += "Remove-Item -LiteralPath $MyInvocation.MyCommand.Path -Force -ErrorAction SilentlyContinue\r\n";

    const QString scriptPath = QDir::toNativeSeparators(
        QStandardPaths::writableLocation(QStandardPaths::TempLocation) + "/vulpurr-update.ps1");
    QFile f(scriptPath);
    if (!f.open(QIODevice::WriteOnly | QIODevice::Truncate))
        return false;
    f.write("\xEF\xBB\xBF");
    f.write(script.toUtf8());
    f.close();

    QSettings settings;
    const int attempts = settings.value("updater/attemptVersion").toInt() == m_readyVersion ? settings.value("updater/attempts").toInt() : 0;
    settings.setValue("updater/attemptVersion", m_readyVersion);
    settings.setValue("updater/attempts", attempts + 1);
    settings.sync();

    const bool started = QProcess::startDetached("powershell.exe",
        {"-NoProfile", "-ExecutionPolicy", "Bypass", "-WindowStyle", "Hidden", "-File", scriptPath});
    qInfo() << "Vulpurr update installer started:" << started << scriptPath;
    return started;
#elif defined(Q_OS_LINUX) || defined(Q_OS_MACOS)
    const QString appDir = QCoreApplication::applicationDirPath();
    const QString walletPath = QCoreApplication::applicationFilePath();
    const QString stageDir = m_stageDir;
    const bool updatesDaemon = m_readyFiles.contains(kDaemonName);

    QString script;
    script += "#!/bin/sh\n";
    script += "LOG=\"${TMPDIR:-/tmp}/vulpurr-update.log\"\n";
    script += "L() { printf '%s %s\\n' \"$(date -u +%Y-%m-%dT%H:%M:%SZ)\" \"$1\" >> \"$LOG\"; }\n";
    script += QString("L 'installing build %1'\n").arg(m_readyVersion);
    script += "APP=" + shQuote(appDir) + "\n";
    script += "STAGE=" + shQuote(stageDir) + "\n";
    script += QString("PID=%1\n").arg(QCoreApplication::applicationPid());
    script += "i=0; while kill -0 \"$PID\" 2>/dev/null && [ $i -lt 60 ]; do sleep 1; i=$((i+1)); done\n";
    script += "L 'wallet closed'\n";
    if (updatesDaemon)
    {
        script += "if [ -x \"$APP/vulpurrd\" ]; then\n";
        script += "  \"$APP/vulpurrd\" --rpc-bind-port 27081 exit >/dev/null 2>&1 || true\n";
        script += "  pkill -f \"$APP/vulpurrd\" >/dev/null 2>&1 || true\n";
        script += "  sleep 2\n";
        script += "fi\n";
    }
    if (m_readyFiles.contains(kP2PoolName))
        script += "pkill -f \"$APP/vulpurr-p2pool\" >/dev/null 2>&1 || true\n";
    script += "ok=1\n";
    script += "put() {\n";
    script += "  mkdir -p \"$(dirname \"$2\")\" || return 1\n";
    script += "  cp -f \"$1\" \"$2.new\" && chmod 755 \"$2.new\" && mv -f \"$2.new\" \"$2\"\n";
    script += "}\n";
    for (const QString &name : m_readyFiles)
    {
        script += QString("if put %1 %2; then L \"updated %3\"; else L \"failed %3\"; ok=0; fi\n")
            .arg(shQuote(stageDir + "/" + name), shQuote(targetPath(name)), name);
    }
#ifdef Q_OS_MACOS
    script += "BUNDLE=\"$(cd \"$APP/../..\" 2>/dev/null && pwd)\"\n";
    script += "case \"$BUNDLE\" in *.app) codesign --force --deep --sign - \"$BUNDLE\" >/dev/null 2>&1 || true; open -n \"$BUNDLE\" || (" + shQuote(walletPath) + " >/dev/null 2>&1 &) ;; *) " + shQuote(walletPath) + " >/dev/null 2>&1 & ;; esac\n";
#else
    script += shQuote(walletPath) + " >/dev/null 2>&1 &\n";
#endif
    script += "if [ \"$ok\" = 1 ]; then rm -rf \"$STAGE\"; fi\n";
    script += "L \"done, ok=$ok\"\n";
    script += "rm -f \"$0\"\n";

    const QString tmpBase = QStandardPaths::writableLocation(QStandardPaths::TempLocation);
    const QString scriptPath = (tmpBase.isEmpty() ? QStringLiteral("/tmp") : tmpBase) + "/vulpurr-update.sh";
    QFile f(scriptPath);
    if (!f.open(QIODevice::WriteOnly | QIODevice::Truncate))
        return false;
    f.write(script.toUtf8());
    f.close();
    f.setPermissions(QFileDevice::ReadOwner | QFileDevice::WriteOwner | QFileDevice::ExeOwner);

    QSettings settings;
    const int attempts = settings.value("updater/attemptVersion").toInt() == m_readyVersion ? settings.value("updater/attempts").toInt() : 0;
    settings.setValue("updater/attemptVersion", m_readyVersion);
    settings.setValue("updater/attempts", attempts + 1);
    settings.sync();

    const bool started = QProcess::startDetached("/bin/sh", {scriptPath});
    qInfo() << "Vulpurr update installer started:" << started << scriptPath;
    return started;
#else
    return false;
#endif
}

bool VulpurrUpdater::downloadVerifiedFile(const QString &name, const QString &destination, QString &error)
{
    try
    {
        Network network;
        QString baseUrl;
        QJsonObject root;
        std::string fetchError;
        if (!fetchManifest(network, baseUrl, root, fetchError))
            throw std::runtime_error(fetchError);

        for (const QJsonValue &value : platformFiles(root))
        {
            const QJsonObject file = value.toObject();
            if (file.value("name").toString() != name)
                continue;
            const QString relUrl = file.value("url").toString(name);
            const QByteArray data = QByteArray::fromStdString(network.get(baseUrl + relUrl));
            if (sha256Hex(data) != file.value("sha256").toString().toLatin1().toLower())
                throw std::runtime_error("hash mismatch");
            QDir().mkpath(QFileInfo(destination).absolutePath());
            QFile out(destination);
            if (!out.open(QIODevice::WriteOnly | QIODevice::Truncate) || out.write(data) != data.size())
                throw std::runtime_error("cannot save file");
#ifndef Q_OS_WIN
            out.setPermissions(QFileDevice::ReadOwner | QFileDevice::WriteOwner | QFileDevice::ExeOwner |
                               QFileDevice::ReadGroup | QFileDevice::ExeGroup |
                               QFileDevice::ReadOther | QFileDevice::ExeOther);
#endif
            return true;
        }
        throw std::runtime_error("file is not published");
    }
    catch (const std::exception &e)
    {
        error = QString::fromStdString(e.what());
        qWarning() << "Vulpurr download of" << name << "failed:" << error;
        return false;
    }
}
