import QtQuick 2.12
import QtQuick.Window 2.15
import QtQuick.Controls 2.15
import QtQuick.Dialogs 1.3

Window {
   visible: true
   width: 670
   height: 400
   title: "FTP Client"


   Row {
       id : inforText
       anchors{
           left: parent.left
           top: parent.top
           leftMargin: 10
           topMargin: 10
       }
       width :560
       spacing: 10

       TextField {
           id: urlField
           width: parent.width/4
           placeholderText: "FTP URL"
       }

       TextField {
           id: usernameField
           width: parent.width/4
           placeholderText: "Username"
       }

       TextField {
           id: passwordField
           width: parent.width/4
           placeholderText: "Password"
           echoMode: TextInput.Password
       }

       TextField {
           id: serverPortNumberField
           width: parent.width/12
           placeholderText: "Port"
       }

       Button {
           width: parent.width/4
           text: "Connect to server"
           onClicked: {
               FtpClient.setFtpServerAddress(urlField.text)
               FtpClient.setFtpServerPortNumber(serverPortNumberField.text)
               FtpClient.setFtpUsername(usernameField.text)
               FtpClient.setFtpPassword(passwordField.text)
               FtpClient.getListFileFromFTPServer()

           }
       }
   }

   Rectangle{
       id : listFileCover
       width: 650
       height : 300
       anchors{
           top: inforText.bottom
           topMargin: 10
           left: parent.left
           leftMargin: 10
       }
       border{
           color: "black"
           width : 5}
       ListView {
           id: listFtp
           anchors{
           top: parent.top
           topMargin: 20
           horizontalCenter: parent.horizontalCenter
           }
           width: parent.width-20
           height: parent.height-50
           model: FtpClient.fileList

           delegate: Rectangle {
               id: itemNow
               width: listFtp.width
               height: 20
               border.color: "gray"

               Text {
                   id:fileName
                   text: modelData
               }
               MouseArea{
                   anchors.fill: parent
                   onClicked: {
                       listFtp.currentIndex = index
                       popup.x = mouseX
                       popup.y = mouseY
                       popup.open()
                   }

               }
               Menu {
                   id: popup
                   MenuItem {
                       text: "Download"
                       onTriggered:{
                           fileDialog.open()
                           console.log("Downloading " + listFtp.model[listFtp.currentIndex])
                       }
                   }
                   MenuItem {
                       text: "Rename"
                       onTriggered: {
                                   renameDialog.open()
                                   renameTextField.text = listFtp.model[listFtp.currentIndex] // Gán tên cũ vào ô nhập
                               }
                   }
                   MenuItem {
                       text: "Delete"
                       onTriggered: {console.log("Deleting " + listFtp.model[listFtp.currentIndex])
                       FtpClient.deleteFileFromFTPServer(listFtp.model[listFtp.currentIndex].toString())}
                   }
               }
            }
           }
       }

   FileDialog {
           id: fileDialog
           title: "Select a folder to save"
           selectFolder: true

           onAccepted: {
               var fileNameTosave = listFtp.model[listFtp.currentIndex]
               var partToSave = fileDialog.folder.toString()
               partToSave = partToSave.substring(7) +"/"+fileNameTosave
               console.log("Selected link to save file: " + partToSave)
               FtpClient.downloadFTPFile(fileNameTosave,partToSave)
           }

           onRejected: {
               console.log("File selection canceled")
           }
       }
   Popup {
       id: renameDialog
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
           }

           Row {
               spacing: 10
               Button {
                   text: "OK"
                   onClicked: {
                       let oldName = listFtp.model[listFtp.currentIndex]
                       let newName = renameTextField.text.trim()
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
    }

