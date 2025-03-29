#include "FtpController.h"

FtpController::FtpController(QObject *parent) : QObject(parent)
{
    ftp = new QFtp(this);
//    ftpServerAddress = "ftp.dlptest.com";
//    ftpServerPortNumber = 21;
//    ftpUsername = "dlpuser";
//    ftpPassword = "rNrKYTX9g7z3RgJRmxWuGHbeu";
    connect(ftp, &QFtp::listInfo, this, &FtpController::onListInfo);
    connect(ftp, &QFtp::commandFinished, this, &FtpController::onCommandFinished);

}


void FtpController::getListFileFromFTPServer()
{
     if (ftp->state()  == QFtp::Unconnected) {
          qDebug() << "Connecting to FTP server...";
          ftp->connectToHost(ftpServerAddress, ftpServerPortNumber);
          ftp->login(ftpUsername, ftpPassword);
     }
          ftp->list();

}

void FtpController::uploadFileToFTPServer(const QString &localFilePath, const QString &remoteFilePath)
{
   QFile *file = new QFile(localFilePath);
   if (!file->open(QIODevice::ReadOnly)) {
       qDebug() << "Failed to open file for upload:" << localFilePath;
       delete file;
       return;
   }
   qDebug() << "Uploading file:" << localFilePath << "to" << remoteFilePath;
   ftp->put(file, remoteFilePath);
}

void FtpController::downloadFTPFile(const QString &ftpFilePath, const QString &downloadFilePath)
{
   qDebug() << "File " << ftpFilePath << " is downloading...";
   downloadFile = new QFile(downloadFilePath);
   if (!downloadFile->open(QIODevice::WriteOnly)) {
       qDebug() << "Failed to open file for writing:" << downloadFilePath;
       delete downloadFile;
       downloadFile = nullptr;
       return;
   }
   if (ftp->state()  == QFtp::Unconnected) {
   ftp->connectToHost(ftpServerAddress, ftpServerPortNumber);
   ftp->login(ftpUsername, ftpPassword);
   }
   ftp->get(ftpFilePath, downloadFile);
}

void FtpController::setFtpServerAddress(QString ftpServerAddress)
{
   qDebug()<< "setFtpServerAddress :"<< ftpServerAddress;
   this->ftpServerAddress = ftpServerAddress;
}

void FtpController::setFtpServerPortNumber(int ftpServerPortNumber)
{
   qDebug()<< "setFtpServerPortNumber :"<< ftpServerPortNumber;
   this->ftpServerPortNumber = ftpServerPortNumber;
}

void FtpController::setFtpUsername(QString ftpUsername)
{
   qDebug()<< "setFtpUsername :"<< ftpUsername;
   this->ftpUsername = ftpUsername;
}

void FtpController::setFtpPassword(QString ftpPassword)
{
   qDebug()<< "setFtpPassword :"<< ftpPassword;
   this->ftpPassword = ftpPassword;
}

void FtpController::deleteFileFromFTPServer(const QString &ftpFilePath)
{
    qDebug() << "Deleting file:" << ftpFilePath;
    if (ftp->state()  == QFtp::Unconnected) {
    ftp->connectToHost(ftpServerAddress, ftpServerPortNumber);
    ftp->login(ftpUsername, ftpPassword);
    }
    ftp->remove(ftpFilePath);
    ftp->list();
}

void FtpController::renameFileOnFTPServer(const QString &oldFilePath, const QString &newFilePath)
{
    qDebug() << "Renaming file from" << oldFilePath << "to" << newFilePath;
    if (ftp->state()  == QFtp::Unconnected) {
    ftp->connectToHost(ftpServerAddress, ftpServerPortNumber);
    ftp->login(ftpUsername, ftpPassword);
    }
    ftp->rename(oldFilePath, newFilePath);
    ftp->list();
}

void FtpController::addFileToList(const QString &fileName)
{
        m_fileList.append(fileName);
        emit fileListChanged();
}

QStringList FtpController::getFileList() const
{
    return m_fileList;
}

void FtpController::onListInfo(const QUrlInfo &info)
{
   addFileToList(info.name()); // Thêm file vào danh sách
}

void FtpController::onCommandFinished(int commandId, bool error)
{
   if (error) {
    qDebug() << "FTP command failed: " << ftp->errorString();
       } else {
           qDebug() << "FTP command finished successfully.";
       }

       // Chỉ hiển thị danh sách file khi lệnh `list` hoàn thành
//       if (ftp->currentCommand() == QFtp::List) {
//           qDebug() << "List of files:";
//           for (const QString &fileName : m_fileList) {
//               qDebug() << "- " << fileName;
//           }
//       }
}


