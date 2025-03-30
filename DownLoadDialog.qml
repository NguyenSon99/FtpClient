import QtQuick 2.0
import QtQuick.Dialogs 1.3

FileDialog {
        id: fileDialog
        title: "Select a folder to save"
        selectFolder: true
        property string fileNameTosave : ""
        property string partToSave: ""

        onAccepted: {
            var partToSave = fileDialog.folder.toString().substring(7) +"/"+fileNameTosave
            console.log("Selected link to save file: " + partToSave)
            FtpClient.downloadFTPFile(fileNameTosave,partToSave)
        }

        onRejected: {
            console.log("File selection canceled")
        }
    }
