import QtQuick 2.13
import QtQuick.Dialogs 1.3
import QtQuick.Controls 2.15

Row {
    spacing: 10
    Button {
        text: "Upload File"
        onClicked: fileDialog.open()
    }

    TextField {
        id: filePath
        width: 200
        placeholderText: "Select a file..."
        readOnly: true
    }

    FileDialog {
        id: fileDialog
        title: "Choose a file to upload"
        onAccepted: {
            filePath.text = fileDialog.fileUrls[0]
            FtpClient.uploadFileToFTPServer(filePath.text, "remote_path_here")
        }
    }
}
