import QtQuick 2.12
import QtQuick.Controls 2.15

ApplicationWindow {
    id: logWindow
    width: 600
    height: 500
    visible: true
    title: "History"

    ListModel {
        id: logModel
    }

    Column {
        anchors.fill: parent
        spacing: 10
        padding: 10

        Button {
            text: "Reload Log"
            onClicked: {
                logModel.clear();
                var logs = FtpClient.readLogFile();
                for (var i = 0; i < logs.length; i++) {
                    logModel.append({ logText: logs[i] });
                }
            }
        }

        ListView {
            id: logListView
            width: parent.width
            height: parent.height - 50
            model: logModel
            spacing: 5
            clip: true

            delegate: Rectangle {
                width: logListView.width
                height: 30
                color: "#333"
                radius: 5
                border.color: "#555"

                Text {
                    anchors.centerIn: parent
                    color: "white"
                    text: model.logText
                    font.pixelSize: 14
                }
            }
        }
    }

    Component.onCompleted: {
        logModel.clear();
        var logs = ftpController.readLogFile();
        for (var i = 0; i < logs.length; i++) {
            logModel.append({ logText: logs[i] });
        }
    }
}
