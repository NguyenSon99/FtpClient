#include "FtpController.h"

FtpController::FtpController(QObject *parent) : QObject(parent)
{
    ftp = new QFtp(this);
//  ftpServerAddress = "ftp.dlptest.com";
//  ftpServerPortNumber = 21;
//  ftpUsername = "dlpuser";
//  ftpPassword = "rNrKYTX9g7z3RgJRmxWuGHbeu";
    connect(ftp, &QFtp::listInfo, this, &FtpController::onListInfo);
    connect(ftp, &QFtp::commandFinished, this, &FtpController::onCommandFinished);
    connect(this, &FtpController::newLogMessage, this, &FtpController::displayLog);

}


void FtpController::getListFileFromFTPServer()
{
    if (ftp->state()  == QFtp::Unconnected) {
        emit newLogMessage( "Connecting to FTP server...");
        ftp->connectToHost(ftpServerAddress, ftpServerPortNumber);
        ftp->login(ftpUsername, ftpPassword);
    }
    emit newLogMessage( "Update list file");
    m_fileList.clear();
    emit fileListChanged();
    ftp->list();

}

void FtpController::uploadFileToFTPServer(const QString &localFilePath)
{
    QFile *file = new QFile(localFilePath);
    if (!file->open(QIODevice::ReadOnly)) {
        emit newLogMessage( "Failed to open file for upload:" + localFilePath);
        delete file;
        return;
    }
    emit newLogMessage(  "Uploading file: " +localFilePath + " to " + this->ftpServerAddress);
    ftp->setTransferMode(QFtp::Passive);
    qDebug() << "📤 Starting upload...";
    QString remoteFileName = QFileInfo(localFilePath).fileName();
    ftp->put(file, remoteFileName);
    m_fileList.clear();
    emit fileListChanged();
    ftp->list();

}

void FtpController::downloadFTPFile(const QString &ftpFilePath, const QString &downloadFilePath)
{
    emit newLogMessage( "File " + ftpFilePath + " is downloading...");
    downloadFile = new QFile(downloadFilePath);
    if (!downloadFile->open(QIODevice::WriteOnly)) {
        emit newLogMessage(  "Failed to open file for writing:" + downloadFilePath);
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
    emit newLogMessage( "Deleting file: " + ftpFilePath);
    if (ftp->state()  == QFtp::Unconnected) {
    ftp->connectToHost(ftpServerAddress, ftpServerPortNumber);
    ftp->login(ftpUsername, ftpPassword);
    }
    ftp->remove(ftpFilePath);
    m_fileList.clear();
    emit fileListChanged();
    ftp->list();
}

void FtpController::renameFileOnFTPServer(const QString &oldFilePath, const QString &newFilePath)
{
    emit newLogMessage(  "Renaming file from " + oldFilePath + " to " + newFilePath);
    if (ftp->state()  == QFtp::Unconnected) {
    ftp->connectToHost(ftpServerAddress, ftpServerPortNumber);
    ftp->login(ftpUsername, ftpPassword);
    }
    ftp->rename(oldFilePath, newFilePath);
    m_fileList.clear();
    emit fileListChanged();
    ftp->list();
}

void FtpController::addFileToList(const QString &fileName)
{
    m_fileList.append(fileName);
    emit fileListChanged();
}

void FtpController::displayLog(const QString &log)
{
    qDebug()<<log;
}

QStringList FtpController::getFileList() const
{
    return m_fileList;
}

void FtpController::onListInfo(const QUrlInfo &info)
{
   addFileToList(info.name()); // Thêm file vào danh sách
}

void FtpController::onCommandFinished( int commandId,bool error)
{
    if (error) {
        QString errorMessage = "FTP command failed: " + ftp->errorString();
        qDebug() << errorMessage;

        switch (ftp->currentCommand()) {
            case QFtp::ConnectToHost:
                emit newLogMessage("❌ Error: Could not connect to FTP server.");
                break;
            case QFtp::Login:
                emit newLogMessage("❌ Error: Incorrect FTP username or password.");
                break;
            case QFtp::List:
                emit newLogMessage("❌ Error: Unable to retrieve file list.");
                break;
            case QFtp::Get:
                emit newLogMessage("❌ Error: Failed to download file.");
                break;
            case QFtp::Put:
                emit newLogMessage("❌ Error: Failed to upload file.");
                break;
            case QFtp::Remove:
                emit newLogMessage("❌ Error: Failed to delete file.");
                break;
            case QFtp::Rename:
                emit newLogMessage("❌ Error: Failed to rename file.");
                break;
            default:
                emit newLogMessage("❌ Error: Unknown FTP error.");
                break;
        }
    } else {
        qDebug() << "✅ FTP command finished successfully.";
//        emit newLogMessage("✅ FTP command finished successfully.");
    }
}


