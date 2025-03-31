import QtQuick 2.13
import QtQuick.Dialogs 1.3
import QtQuick.Controls 2.15

Row {
    width: 560
    spacing: 10

    TextField {
        id: filePath
        width: 440
        placeholderText: "Select a file..."
        readOnly: true
    }

    Button {
        width : 100
        text: "Upload File"
        onClicked: fileDialog.open()
    }


    FileDialog {
        id: fileDialog
        title: "Choose a file to upload"
        onAccepted: {
            filePath.text = fileDialog.fileUrls[0].toString().substring(7)
            console.log(filePath.text)
            FtpClient.uploadFileToFTPServer(filePath.text)
        }
    }
}
