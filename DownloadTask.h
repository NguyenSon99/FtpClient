#ifndef DOWNLOADTASK_H
#define DOWNLOADTASK_H


#include <QObject>
#include <QFtp>
#include <QFile>
#include <QThread>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <QDebug>
class DownloadTask : public QThread
{
    Q_OBJECT

public:
    bool downLoadDone;
    explicit DownloadTask(const QString &ftpServer, int port, const QString &username, const QString &password,
                          const QString &ftpFilePath, const QString &downloadFilePath, QObject *parent = nullptr)
        : QThread(parent), server(ftpServer), port(port), user(username), pass(password),
          ftpPath(ftpFilePath), localPath(downloadFilePath) {}

signals:
    void logMessage(const QString &message);

protected:
    void run() override {
        char buffer[1024];
        int controlSocket;
        int dataSocket;
        emit logMessage("File " + ftpPath + " is downloading...");

        // 1. Tạo socket điều khiển
        controlSocket = socket(AF_INET, SOCK_STREAM, 0);
        if (controlSocket < 0) {
            emit logMessage("Không thể tạo control socket.");
            return;
        }

        // 2. Kết nối tới server FTP
        sockaddr_in serverAddr{};
        serverAddr.sin_family = AF_INET;
        serverAddr.sin_port = htons(port);
        inet_pton(AF_INET, server.toStdString().c_str(), &serverAddr.sin_addr);

        if (::connect(controlSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
            emit logMessage("Không thể kết nối tới server.");
            return;
        }

        // 3. Đọc chào mừng từ server
        memset(buffer, 0, sizeof(buffer));
        recv(controlSocket, buffer, sizeof(buffer), 0);
        emit logMessage(QString::fromStdString(buffer));

        auto sendCommand = [&](const std::string& cmd) -> std::string {
            std::string fullCmd = cmd + "\r\n";
            send(controlSocket, fullCmd.c_str(), fullCmd.size(), 0);
            memset(buffer, 0, sizeof(buffer));
            recv(controlSocket, buffer, sizeof(buffer), 0);
            return std::string(buffer);
        };

        // 4. Đăng nhập
        std::string resp = sendCommand("USER " + user.toStdString());
        emit logMessage(QString::fromStdString(resp));
        resp = sendCommand("PASS " + pass.toStdString());
        emit logMessage(QString::fromStdString(resp));

        if (resp.find("230") == std::string::npos) {
            emit logMessage("Đăng nhập thất bại.");
            return;
        }

        // 5. Bật chế độ PASV

    // 6. Mở kết nối dữ liệu
                 resp = sendCommand("PASV");
              if (resp.find("227") == std::string::npos) {
                  emit logMessage( "Lỗi khi chuyển sang chế độ PASV: " + QString::fromStdString( resp));
                  return ;
              }

              // Phân tích phản hồi để lấy địa chỉ IP và cổng
              size_t start = resp.find('(');
              size_t end = resp.find(')');
              std::string ipAndPort = resp.substr(start + 1, end - start - 1);

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

                 // 6. Mở kết nối dữ liệu
              std::string serverIP = std::to_string(values[0]) + "." +
                                     std::to_string(values[1]) + "." +
                                     std::to_string(values[2]) + "." +
                                     std::to_string(values[3]);
              int serverPort = values[4] * 256 + values[5];

              // Tạo socket dữ liệu
              dataSocket = socket(AF_INET, SOCK_STREAM, 0);
              if (dataSocket < 0) {
                  qDebug() << "Không thể tạo socket dữ liệu.";
                  return ;
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
                  return ;
              }

        // 7. Gửi lệnh RETR để tải file
        resp = sendCommand("RETR " + ftpPath.toStdString());
        emit logMessage(QString::fromStdString(resp));
        if (resp.find("150") == std::string::npos && resp.find("125") == std::string::npos) {
            emit logMessage("Không thể tải file.");
            close(dataSocket);
            return;
        }

        // 8. Ghi dữ liệu ra file
        QFile file(localPath);
        if (!file.open(QIODevice::WriteOnly)) {
            emit logMessage("Không thể mở file để ghi.");
            emit logMessage(localPath);
            close(dataSocket);
            return;
        }

        int bytesRead;
        while ((bytesRead = recv(dataSocket, buffer, sizeof(buffer), 0)) > 0) {
            file.write(buffer, bytesRead);
        }
        file.close();
        close(dataSocket);

        //9. Nhận phản hồi cuối cùng
        memset(buffer, 0, sizeof(buffer));
        recv(controlSocket, buffer, sizeof(buffer), 0);
        emit logMessage(QString::fromStdString(buffer));

        if (strstr(buffer, "226") == nullptr) {
            downLoadDone = false;
            emit logMessage("Tải file thất bại.");
        } else {
            downLoadDone = true;
            emit logMessage("Tải file thành công: " + ftpPath);
        }

        close(controlSocket);
    }

private:
    QString server, user, pass, ftpPath, localPath;
    int port;
};
#endif // DOWNLOADTASK_H
