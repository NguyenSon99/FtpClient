#ifndef FTPCONTROLLER_H
#define FTPCONTROLLER_H

#include <QCoreApplication>
#include <QDebug>
#include <QFtp>
#include <QFile>
#include <QStringList>

class FtpController : public QObject
{
    Q_OBJECT
private:
    QFtp *ftp;
    QString ftpServerAddress = "ftp.dlptest.com";
       int ftpServerPortNumber = 21;
       QString ftpUsername = "dlpuser";
       QString ftpPassword = "rNrKYTX9g7z3RgJRmxWuGHbeu";
        QStringList fileList;

public:
    explicit FtpController(QObject *parent = nullptr);
//       void connectFTPServer();
        void getListFileFromFTPServer();

signals:
private slots:
    void onListInfo(const QUrlInfo &info) {
        fileList.append(info.name()); // Thêm file vào danh sách
    }

    void     onCommandFinished(int commandId, bool error) {
            if (error) {
                qDebug() << "FTP command failed: " << ftp->errorString();
            } else {
                qDebug() << "FTP command finished successfully.";
            }

            // Chỉ hiển thị danh sách file khi lệnh `list` hoàn thành
            if (ftp->currentCommand() == QFtp::List) {
                qDebug() << "List of files:";
                for (const QString &fileName : fileList) {
                    qDebug() << "- " << fileName;
                }
            }
        }
};

#endif // FTPCONTROLLER_H
