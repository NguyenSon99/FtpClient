#include "FtpController.h"

FtpController::FtpController(QObject *parent) : QObject(parent)
{
      ftp = new QFtp(this);
      QString ftpServerAddress = "ftp.dlptest.com";
         int ftpServerPortNumber = 21;
         QString ftpUsername = "dlpuser";
         QString ftpPassword = "rNrKYTX9g7z3RgJRmxWuGHbeu";
         connect(ftp, &QFtp::listInfo, this, &FtpController::onListInfo);
                 connect(ftp, &QFtp::commandFinished, this, &FtpController::onCommandFinished);

}

//void FtpController::connectFTPServer()
//{
//    qDebug() << "Testing FTP connection...";
//    ftp->connectToHost(ftpServerAddress, ftpServerPortNumber);
//    ftp->login(ftpUsername, ftpPassword);
//     disconnect(ftp, &QFtp::commandFinished, nullptr, nullptr);
//            connect(ftp, &QFtp::commandFinished, this, [&](int, bool error) {
//                if (error) {
//                    qDebug() << "FTP connection test failed: " << ftp->errorString();
//                } else {
//                    qDebug() << "FTP connection successful.";
//                }
//                ftp->close();
//            });
//            if (ftp->currentCommand() == QFtp::Login) {
//                       ftp->close();
//                   }

//}

void FtpController::getListFileFromFTPServer()
{
    qDebug() << "Connecting to FTP server...";
           ftp->connectToHost(ftpServerAddress, ftpServerPortNumber);
           ftp->login(ftpUsername, ftpPassword);
           ftp->list();

}
