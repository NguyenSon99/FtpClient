import QtQuick.Controls 2.15
import QtQuick.Dialogs 1.3
import QtQuick 2.12

Popup {
    id: renameDialog

    property string oldName
    property string newName
    width: 300
    height: 150
    modal: true
    focus: true
    closePolicy: Popup.CloseOnEscape

    Column {
        anchors.centerIn: parent
        spacing: 10

        Text {
            text: "Enter new name:"
        }

        TextField {
            id: renameTextField
            width: 200
            text: oldName
        }

        Row {
            spacing: 10
            Button {
                text: "OK"
                onClicked: {
                    newName = renameTextField.text.trim()
                    if (newName !== "" && newName !== oldName) {
                        FtpClient.renameFileOnFTPServer(oldName, newName)
                        renameDialog.close()
                    }
                }
            }
            Button {
                text: "Cancel"
                onClicked: renameDialog.close()
            }
        }
    }
 }
