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

bool FtpController::openDataConnection()
{
    std::string response = sendCommand("PASV");
          if (response.find("227") == std::string::npos) {
              qDebug()<< "Lỗi khi chuyển sang chế độ PASV: " + QString::fromStdString( response);
              return false;
          }

          // Phân tích phản hồi để lấy địa chỉ IP và cổng
          size_t start = response.find('(');
          size_t end = response.find(')');
          std::string ipAndPort = response.substr(start + 1, end - start - 1);

          // Phân tích thành các con số
          size_t commaPos;
          std::string token;
          int values[6];
          int i = 0;
          std::string temp = ipAndPort;

          while ((commaPos = temp.find(',')) != std::string::npos) {
              token = temp.substr(0, commaPos);
              values[i++] = std::stoi(token);
              temp = temp.substr(commaPos + 1);
          }
          values[i] = std::stoi(temp);

          // Tính toán địa chỉ IP và cổng
          std::string serverIP = std::to_string(values[0]) + "." +
                                 std::to_string(values[1]) + "." +
                                 std::to_string(values[2]) + "." +
                                 std::to_string(values[3]);
          int serverPort = values[4] * 256 + values[5];

          // Tạo socket dữ liệu
          dataSocket = socket(AF_INET, SOCK_STREAM, 0);
          if (dataSocket < 0) {
              qDebug() << "Không thể tạo socket dữ liệu.";
              return false;
          }

          // Thiết lập kết nối dữ liệu
          struct sockaddr_in dataAddr;
          memset(&dataAddr, 0, sizeof(dataAddr));
          dataAddr.sin_family = AF_INET;
          dataAddr.sin_port = htons(serverPort);
          inet_pton(AF_INET, serverIP.c_str(), &dataAddr.sin_addr);

          if (::connect(dataSocket, (struct sockaddr*)&dataAddr, sizeof(dataAddr)) < 0) {
              qDebug() << "Không thể kết nối đến cổng dữ liệu.";
              close(dataSocket);
              return false;
          }

          return true;
}

void FtpController::closeDataConnection()
{
    if (dataSocket >= 0) {
              close(dataSocket);
              dataSocket = -1;
    }
}

bool FtpController::connectToServer()
{
    emit newLogMessage( "Connecting to FTP server...");
    // Tạo socket
    controlSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (controlSocket < 0) {
        qDebug() << "Không thể tạo socket.";
        return false;
    }

    // Thiết lập thông tin server
    struct sockaddr_in serverAddr;
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(ftpServerPortNumber);

    // Lấy địa chỉ IP từ hostname
    struct hostent *server = gethostbyname(ftpServerAddress.toStdString().c_str());
    if (server == NULL) {
        displayLog( "Không tìm thấy host: " + ftpServerAddress );
        close(controlSocket);
        return false;
    }

    memcpy(&serverAddr.sin_addr.s_addr, server->h_addr, server->h_length);

    // Kết nối đến server
    if (::connect(controlSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        qDebug() << "Không thể kết nối đến server.";
        close(controlSocket);
        return false;
    }

    // Nhận phản hồi chào mừng
    memset(buffer, 0, sizeof(buffer));
    recv(controlSocket, buffer, sizeof(buffer), 0);
    qDebug() << QString::fromStdString( buffer);

    isConnected = true;
    return true;
}

bool FtpController::login()
{
    if (!isConnected) {
                displayLog( "Chưa kết nối đến server FTP!");
                return false;
            }

            // Gửi lệnh USER
            std::string response = sendCommand("USER " + ftpUsername.toStdString());
            qDebug() << QString::fromStdString(response);

            if (response.find("331") == std::string::npos) {
                qDebug() <<  "Lỗi khi gửi username.";
                return false;
            }

            // Gửi lệnh PASS
            response = sendCommand("PASS " + ftpPassword.toStdString());
            qDebug() << QString::fromStdString(response);

            if (response.find("230") == std::string::npos) {
                qDebug()  << "Lỗi khi gửi password.";
                return false;
            }

            isLoggedIn = true;
            return true;
}

void FtpController::getListFileFromFTPServer()
{

    if (!isLoggedIn) {
        qDebug()  << "Chưa đăng nhập vào server FTP!";
        return;
    }

    // Mở kết nối dữ liệu
    if (!openDataConnection()) {
        qDebug() << "Không thể mở kết nối dữ liệu.";
        return;
    }

    // Gửi lệnh LIST
    std::string response = sendCommand("LIST");
    if (response.find("150") == std::string::npos && response.find("125") == std::string::npos) {
        qDebug()  << "Lỗi khi gửi lệnh LIST: " << QString::fromStdString(response);
        closeDataConnection();
        return;
    }

    memset(buffer, 0, sizeof(buffer));
    int bytesRead;
    std::string result;

    while ((bytesRead = recv(dataSocket, buffer, sizeof(buffer) - 1, 0)) > 0) {
        buffer[bytesRead] = '\0';
        result += buffer;
        memset(buffer, 0, sizeof(buffer));
    }
    // Đóng dataSocket sau khi nhận xong dữ liệu ; Đó là socket tạm thời, chỉ dùng cho một lệnh. Server đã đóng nó ở phía bên kia. Tránh rò rỉ tài nguyên.
    closeDataConnection();

    // Nhận phản hồi cuối cùng
    memset(buffer, 0, sizeof(buffer));
    recv(controlSocket, buffer, sizeof(buffer), 0);
    qDebug()  <<  QString::fromStdString(buffer);

    // Phân tách thành từng dòng
    emit newLogMessage( "Update list file");
    std::istringstream iss(result); //đọc từng dòng từ chuỗi đó
    std::string line;
    m_fileList.clear();
    while (std::getline(iss, line)) {
        std::istringstream linestream(line);
        std::string token, filename;
        while (linestream >> token) {
            filename = token;  // cuối cùng là tên file
        }
        m_fileList.append(QString::fromStdString(filename));
    }
    emit fileListChanged();
}

void FtpController::uploadFileToFTPServer(const QString &localFilePath)
{

    if (!isLoggedIn) {
              emit newLogMessage( "Chưa đăng nhập vào server FTP!");
            return ;
        }

        // Mở file để đọc
        std::ifstream inFile(localFilePath.toStdString(), std::ios::binary);
        if (!inFile) {
            emit newLogMessage( "Không thể mở file " + localFilePath + " để đọc.");
            return ;
        }

        // Mở kết nối dữ liệu
        if (!openDataConnection()) {
            emit newLogMessage( "Không thể mở kết nối dữ liệu.");
            inFile.close();
            return ;
        }


        // Gửi lệnh STOR
        QString fileName = QFileInfo(localFilePath).fileName();
        std::string response = sendCommand("STOR " + fileName.toStdString());
        if (response.find("150") == std::string::npos && response.find("125") == std::string::npos) {
            qDebug()<< "Lỗi khi gửi lệnh STOR: " <<  QString::fromStdString(response);
            closeDataConnection();
            inFile.close();
            return ;
        }
       emit newLogMessage(  "Uploading file: " +localFilePath + " to " + this->ftpServerAddress);

        // Đọc từ file và gửi qua socket
        while (!inFile.eof()) {
            inFile.read(buffer, sizeof(buffer));
            int bytesRead = inFile.gcount();
            if (bytesRead > 0) {
                send(dataSocket, buffer, bytesRead, 0);
            }
        }

        inFile.close();
        closeDataConnection();

        // Nhận phản hồi cuối cùng
        memset(buffer, 0, sizeof(buffer));
        recv(controlSocket, buffer, sizeof(buffer), 0);
        qDebug() << buffer;

        if (strstr(buffer, "226") == nullptr) {
            qDebug()  << "Lỗi khi tải file lên.";
            return ;
        }

        emit newLogMessage( "Đã tải file " + localFilePath + " lên server " + ftpServerAddress + " thành công.");
        addLogHistory("UPLOAD", localFilePath);
        getListFileFromFTPServer();
        return ;
}

void FtpController::downloadFTPFile(const QString &ftpFilePath, const QString &downloadFilePath)
{

    DownloadTask *task = new DownloadTask(ftpServerAddress, ftpServerPortNumber, ftpUsername, ftpPassword, ftpFilePath, downloadFilePath);

    connect(task, &DownloadTask::logMessage, this, &FtpController::newLogMessage);
    connect(task, &QThread::finished, task, &QObject::deleteLater); // Tự động xoá khi xong

    task->start();
    if(task->downLoadDone){addLogHistory("DOWNLOAD",ftpFilePath+" To" +downloadFilePath);}
    else {}
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
    addLogHistory("DELETE", ftpFilePath);
    m_fileList.clear();
    emit fileListChanged();
    ftp->list();
}

void FtpController::renameFileOnFTPServer(const QString &oldFilePath, const QString &newFilePath)
{
    emit newLogMessage(  "Renaming file from " + oldFilePath + " to " + newFilePath);
    if (!isLoggedIn) {
                std::cout << "Chưa đăng nhập vào server FTP!\n";
                return ;
            }

            // Gửi lệnh RNFR (Rename From)
            std::string response = sendCommand("RNFR " + oldFilePath.toStdString());
            if (response.find("350") == std::string::npos) {
                std::cout << "Lỗi khi gửi lệnh RNFR: " << response;
                return ;
            }

            // Gửi lệnh RNTO (Rename To)
            response = sendCommand("RNTO " + newFilePath.toStdString());
            if (response.find("250") == std::string::npos) {
                std::cout << "Lỗi khi gửi lệnh RNTO: " << response;
                return ;
            }

            qDebug() << "Đã đổi tên file " << oldFilePath << " thành " << newFilePath << " thành công.";
            return ;
}

QStringList FtpController::readLogFile()
{
    QStringList logList;
    QString logDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir().mkpath(logDir);
    QString logFilePath = logDir + "/historyLog.txt";
    qDebug() << logFilePath;
       QFile file(logFilePath);

       if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
           emit newLogMessage( "⚠️ Cannot open history file for reading.");
           return logList;
       }

       QTextStream in(&file);
       while (!in.atEnd()) {
           logList.append(in.readLine());
       }

       file.close();
       return logList;
}

std::string FtpController::sendCommand(const std::string &command)
{
    if (!isConnected) {
        return "Chưa kết nối đến server!";
    }

    // Gửi lệnh đến server
    command.length() > 0 ? send(controlSocket, (command + "\r\n").c_str(), command.length() + 2, 0) : 0;

    // Nhận phản hồi
    memset(buffer, 0, sizeof(buffer));
    recv(controlSocket, buffer, sizeof(buffer), 0);

    return std::string(buffer);
}



void FtpController::addLogHistory(const QString &action, const QString &details)
{
    QString logDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir().mkpath(logDir);

    QString logFilePath = logDir + "/historyLog.txt";
    qDebug() << logFilePath;
    QFile file(logFilePath);
    if (file.open(QIODevice::Append | QIODevice::Text)) {
        QTextStream out(&file);
        QString timestamp = QDateTime::currentDateTime().toString("yyyy-MM-dd HH:mm:ss");
        out << "[" << timestamp << "] " << ftpUsername <<" : "<< action << " - " << details << "\n";
        file.close();
    } else {
        qDebug() << "⚠️ Cannot open history file for writing.";
    }
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
