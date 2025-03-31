import QtQuick.Controls 2.15
import QtQuick 2.12

Rectangle {
    id: logContainer
    width: 650
    height: 100
    border.color: "#555"
    border.width: 1
    radius: 5

    ListModel {
        id: logModel
    }

    ListView {
        id: logView
        anchors{fill: parent
            leftMargin:2}
        clip: true
        model: logModel
        spacing: 2

        delegate:
            Text {
            width: logView.width
            text: modelData
            wrapMode: Text.Wrap
            font.pixelSize: 12
        }

    }


    function addLog(message) {
        logModel.append({ "logText": message })
        logView.positionViewAtEnd() // Cuộn xuống dòng mới nhất

    }


    Component.onCompleted: {
        FtpClient.newLogMessage.connect(addLog)
    }
}
