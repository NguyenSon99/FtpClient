#ifndef FTPCONTROLLER_H
#define FTPCONTROLLER_H

#include <QCoreApplication>
#include <QDebug>
#include <QFtp>
#include <QFile>
#include <QStringList>
#include <QFileInfo>
#include <QTextStream>
#include <QDateTime>
#include <QStandardPaths>
#include <QDir>
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <curl/curl.h>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include "DownloadTask.h"

class FtpController : public QObject
{
   Q_OBJECT
   Q_PROPERTY(QStringList  fileList READ getFileList  NOTIFY fileListChanged)
   QFtp *ftp;
   QString ftpServerAddress ;
   int ftpServerPortNumber ;
   QString ftpUsername ;
   QString ftpPassword ;
   QFile *downloadFile;
   QStringList m_fileList;
   int controlSocket;
   int dataSocket;
   bool isConnected;
   bool isLoggedIn;
   char buffer[1024];
//   CURL *curl;

public:
   explicit FtpController(QObject *parent = nullptr);
   bool openDataConnection();
   void closeDataConnection();
   Q_INVOKABLE bool connectToServer();
   Q_INVOKABLE bool login();
   Q_INVOKABLE void getListFileFromFTPServer();
   Q_INVOKABLE void uploadFileToFTPServer(const QString& localFilePath);
   Q_INVOKABLE void downloadFTPFile(const QString &ftpFilePath, const QString &downloadFilePath);
   Q_INVOKABLE void setFtpServerAddress(QString ftpServerAddress);
   Q_INVOKABLE void setFtpServerPortNumber(int ftpServerPortNumber);
   Q_INVOKABLE void setFtpUsername(QString ftpUsername);
   Q_INVOKABLE void setFtpPassword(QString ftpPassword);
   Q_INVOKABLE void deleteFileFromFTPServer(const QString &ftpFilePath);
   Q_INVOKABLE void renameFileOnFTPServer(const QString &oldFilePath, const QString &newFilePath);
   Q_INVOKABLE QStringList readLogFile();
   Q_INVOKABLE void changeDirectory(const QString &directory );
   Q_INVOKABLE void backDirectory ();
   std::string sendCommand(const std::string &command );
   void addLogHistory(const QString &action, const QString &details);
   QStringList getFileList() const;
   void addFileToList(const QString &fileName);
   void displayLog(const QString &log);

signals:
void fileListChanged();
void newLogMessage(QString message);

private slots:
   void onListInfo(const QUrlInfo &info) ;
   void onCommandFinished(int commandId, bool error) ;
};

#endif // FTPCONTROLLER_H
