#ifndef DOWNLOADTASK_H
#define DOWNLOADTASK_H


#include <QObject>
#include <QFtp>
#include <QFile>
#include <QThread>

class DownloadTask : public QThread
{
    Q_OBJECT

public:
    explicit DownloadTask(const QString &ftpServer, int port, const QString &username, const QString &password,
                          const QString &ftpFilePath, const QString &downloadFilePath, QObject *parent = nullptr)
        : QThread(parent), server(ftpServer), port(port), user(username), pass(password),
          ftpPath(ftpFilePath), localPath(downloadFilePath) {}

signals:
    void logMessage(const QString &message);

protected:
    void run() override {
        emit logMessage("File " + ftpPath + " is downloading...");

        QFtp ftp;
        ftp.connectToHost(server, port);
        ftp.login(user, pass);

        QFile file(localPath);
        if (!file.open(QIODevice::WriteOnly)) {
            emit logMessage("Failed to open file for writing: " + localPath);
            return;
        }

        ftp.get(ftpPath, &file);
        ftp.close();
        emit logMessage("Download completed: " + ftpPath);
    }

private:
    QString server, user, pass, ftpPath, localPath;
    int port;
};


#endif // DOWNLOADTASK_H
