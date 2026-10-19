#pragma once

#include <QObject>
#include <QString>
#include <QStringList>

#include <atomic>

// Vulpurr: signed self-update from the public node
class VulpurrUpdater : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int currentVersion READ currentVersion CONSTANT)
    Q_PROPERTY(bool busy READ busy NOTIFY busyChanged)

public:
    explicit VulpurrUpdater(QObject *parent = nullptr);

    int currentVersion() const;
    bool busy() const;

    // Checks the manifest, verifies its signature and downloads changed files in the background
    Q_INVOKABLE void checkAsync();
    // Starts a detached installer that waits for this process to exit; the caller must quit the app
    Q_INVOKABLE bool applyAndRestart();

    // Downloads one file listed in the signed manifest and verifies its hash (blocking)
    static bool downloadVerifiedFile(const QString &name, const QString &destination, QString &error);

signals:
    void busyChanged() const;
    void updateReady(int version, const QString &notes) const;
    void upToDate() const;
    void failed(const QString &error) const;

private:
    void run();

    std::atomic<bool> m_busy{false};
    int m_readyVersion = 0;
    QStringList m_readyFiles;
    QString m_stageDir;
};
