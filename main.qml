import QtQuick 2.12
import QtQuick.Window 2.15
import QtQuick.Controls 2.15
import QtQuick.Dialogs 1.3

Window {
   visible: true
   width: 670
   height: 500
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

   UploadDialog{
       id : uploadArea
       anchors{
           top: inforText.bottom
           topMargin: 10
           left: parent.left
           leftMargin: 10
       }
   }

   DisplayLog{
       id: logArea
       anchors{
           top: uploadArea.bottom
           topMargin: 10
           left: parent.left
           leftMargin: 10
       }
   }

   Rectangle{
       id : listFileCover
       width: 650
       height : 300
       radius: 5
       anchors{
           top: logArea.bottom
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
                       onTriggered: renameDialog.open()
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

   DownLoadDialog{
       id: fileDialog
       fileNameTosave :  (listFtp.currentIndex >= 0) ? listFtp.model[listFtp.currentIndex] : ""
   }

   RenamePopup{
       id: renameDialog
       oldName:  (listFtp.currentIndex >= 0) ? listFtp.model[listFtp.currentIndex] : ""
   }
 }

