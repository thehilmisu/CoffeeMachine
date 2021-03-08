import QtQuick 2.12
import QtQuick.Window 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12

Item {

    width: parent.width
    height: parent.height

    Component.onCompleted: {
        manager.oldScreenIndex = 0;//StandbyScreen
    }

    Rectangle{
        anchors.fill: parent
        color:"transparent"

        Button{
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.verticalCenter: parent.verticalCenter
            text: "Turn ON"

            onClicked: {
                maker.turnOn();
                manager.activeScreenIndex = 1; //MenuScreen
            }
        }

    }
}
