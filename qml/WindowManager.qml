import QtQuick 2.12
import QtQuick.Window 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12

Item {
    id:manager
    anchors.fill: parent

    property var activeScreenIndex
    property var oldScreenIndex

    onActiveScreenIndexChanged: {
        console.log("screen index set to : "+activeScreenIndex);
        switch(activeScreenIndex) {
            case 0: //StandbyScreen
                btnGoBack.visible = false;
                standbyScreen.visible = true;
                menuScreen.visible = false;
                settingsScreen.visible = false;
                statesScreen.visible = false;
                txtHeader.text = "Standby Screen";

                return "Off";
            case 1: //MenuScreen
                btnGoBack.visible = true;
                btnGoBack.text = "Turn Off";
                standbyScreen.visible = false;
                menuScreen.visible = true;
                settingsScreen.visible = false;
                statesScreen.visible = false;
                txtHeader.text = "Menu Screen";

                return "Off";
            case 2: //SettingsScreen
                btnGoBack.visible = true;
                btnGoBack.text = "Back";
                standbyScreen.visible = false;
                menuScreen.visible = false;
                settingsScreen.visible = true;
                settingsScreen.refreshValues();
                statesScreen.visible = false;
                txtHeader.text = "Settings Screen";

                return "Off";
            case 3: //StatesScreen
                btnGoBack.visible = true;
                btnGoBack.text = "Back";
                standbyScreen.visible = false;
                menuScreen.visible = false;
                settingsScreen.visible = false;
                statesScreen.visible = true;
                txtHeader.text = "States Screen";

                return "Off";

            default: return "Unknown";
        }
    }

    Image {
        id: backgroundImage
        anchors.fill: parent
        source: "images/wooden_background.jpg"


        //****************Constant area*******************************************
        Rectangle{
            id: headerPane
            height: 25
            width: parent.width
            color: "transparent"
            anchors.horizontalCenter: parent.horizontalCenter
            Text {
                id: txtHeader
                anchors.horizontalCenter: parent.horizontalCenter
                text: ""
                font.pointSize: 20
                font.bold: true
                color: "white"
            }


        }
        Button{
            id:btnGoBack
            text: "<<"
            onClicked: {

                if(activeScreenIndex === 1)//menuScreen
                {
                    activeScreenIndex = oldScreenIndex;
                    maker.turnOff();
                }
                else
                {
                    activeScreenIndex = oldScreenIndex;
                }
            }
        }
        Rectangle{
            id: rightPane
            anchors.right: backgroundImage.right
            Button{
                id: btnSettings
                text: "Settings"
                anchors.right: rightPane.left

                onClicked: {
                    console.log("settings..." + headerPane.height)
                    activeScreenIndex = 2; //SettingsScreen
                    oldScreenIndex = 1;

                }
            }

        }
        //****************end of Constant area*******************************************

        //****************Widget area****************************************************

        StandbyScreen{
            id:standbyScreen
            visible: false
            y: (headerPane.height + 10)
            x: 0
        }
        MenuScreen{
            id:menuScreen
            visible: false
            y: (headerPane.height + 10)
            x: 0
        }
        StatesScreen{
            id:statesScreen
            visible: false
            y: (headerPane.height + 10)
            x: 0
        }
        SettingsScreen{
            id:settingsScreen
            visible: false
            y: (headerPane.height + 10)
            x: 0
        }

        //****************end of Widget area*********************************************

    }

}
