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
const QString kWalletName = QStringLiteral("vulpurr-wallet.exe");
const QString kDaemonName = QStringLiteral("vulpurrd.exe");
// Node name before the rename; still published for wallets from builds <= 5
const QString kLegacyDaemonName = QStringLiteral("monerod.exe");
const QString kP2PoolName = QStringLiteral("vulpurr-p2pool.exe");

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

QString psQuote(const QString &s)
{
    return "'" + QString(s).replace("'", "''") + "'";
}

// where each published file lives relative to the wallet
QString targetPath(const QString &name)
{
    const QString appDir = QCoreApplication::applicationDirPath();
    if (name == kWalletName)
        return QCoreApplication::applicationFilePath();
    if (name == kP2PoolName)
        return appDir + "/p2pool/" + name;
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

        const QString appDir = QCoreApplication::applicationDirPath();
        const QString stageDir = appDir + "/vulpurr-update";
        if (!QDir().mkpath(stageDir))
            throw std::runtime_error("cannot write to the wallet folder");

        const QJsonArray files = root.value("files").toArray();
        bool hasNewDaemon = false;
        for (const QJsonValue &value : files)
            hasNewDaemon |= value.toObject().value("name").toString() == kDaemonName;

        QStringList ready;
        for (const QJsonValue &value : files)
        {
            const QJsonObject file = value.toObject();
            const QString name = file.value("name").toString();
            const QByteArray hash = file.value("sha256").toString().toLatin1().toLower();
            if (name != kWalletName && name != kDaemonName && name != kLegacyDaemonName && name != kP2PoolName)
                continue;
            if (name == kLegacyDaemonName && hasNewDaemon)
                continue;

            const QString target = targetPath(name);
            if (fileSha256Hex(target) == hash)
                continue;

            const QString staged = stageDir + "/" + name;
            if (fileSha256Hex(staged) != hash)
            {
                qInfo() << "Vulpurr update: downloading" << name;
                const std::string body = network.get(baseUrl + name);
                const QByteArray data = QByteArray::fromStdString(body);
                if (sha256Hex(data) != hash)
                    throw std::runtime_error(("hash mismatch for " + name).toStdString());
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
            throw std::runtime_error("installing this build failed twice, see %TEMP%\\vulpurr-update.log");

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
#ifdef Q_OS_WIN
    if (m_readyFiles.isEmpty() || m_busy)
        return false;

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

        for (const QJsonValue &value : root.value("files").toArray())
        {
            const QJsonObject file = value.toObject();
            if (file.value("name").toString() != name)
                continue;
            const QByteArray data = QByteArray::fromStdString(network.get(baseUrl + name));
            if (sha256Hex(data) != file.value("sha256").toString().toLatin1().toLower())
                throw std::runtime_error("hash mismatch");
            QDir().mkpath(QFileInfo(destination).absolutePath());
            QFile out(destination);
            if (!out.open(QIODevice::WriteOnly | QIODevice::Truncate) || out.write(data) != data.size())
                throw std::runtime_error("cannot save file");
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
