#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include "FtpController.h"
#include <QQmlContext>

int main(int argc, char *argv[])
{
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
   QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif

   QGuiApplication app(argc, argv);

   FtpController client;
   client.getListFileFromFTPServer();
//QString ftpFilePath = "test.txt";
//QString downloadFilePath =  "/home/test.txt";
//client.downloadFTPFile(ftpFilePath,downloadFilePath);



   QQmlApplicationEngine engine;
   engine.rootContext()->setContextProperty("FtpClient", &client);
   const QUrl url(QStringLiteral("qrc:/main.qml"));
   QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                    &app, [url](QObject *obj, const QUrl &objUrl) {
       if (!obj && url == objUrl)
           QCoreApplication::exit(-1);
   }, Qt::QueuedConnection);
   engine.load(url);

   return app.exec();
}
