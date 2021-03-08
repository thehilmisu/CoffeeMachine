import QtQuick 2.12
import QtQuick.Window 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12

// With this import you have access to special types, like enums
import CoffeeMaker 1.0

Item {


    width: parent.width
    height: parent.height

    Component.onCompleted: {
        manager.oldScreenIndex = 2;//settingsScreen
        refreshValues();
    }

    function refreshValues(){
        var beansState = "Beans: %1/%2".arg(maker.beansContainerLevel).arg(maker.beansContainerMax());
        var waterState = "Water: %1/%2".arg(maker.waterContainerLevel).arg(maker.waterContainerMax());
        var milkState = "Milk: %1/%2".arg(maker.milkContainerLevel).arg(maker.milkContainerMax());
        var overflowState = "Overflow: %1/%2".arg(maker.overflowContainerLevel).arg(maker.overflowContainerMax());
        var restBinState = "RestBin: %1/%2".arg(maker.restBinLevel).arg(maker.restBinLevelMax());

        txtStatus.text = "%1         %3         %4\n%2     %5".arg(beansState).arg(waterState).arg(milkState).arg(overflowState).arg(restBinState);
    }

    Rectangle{
        width: parent.width
        height: 50
        x:0;y:parent.y
        color:"black"
        opacity: 0.5

        Text {
            id: txtStatus
            text: ""
            color: "white"
            font.pointSize: 12
            font.bold: true
            anchors.horizontalCenter: parent.horizontalCenter
            opacity: 1
        }
    }

    GridLayout {
            anchors.fill: parent
            columns: 3
            rowSpacing: 40

            Button {
              text: "Empty bin"
              onClicked: maker.emptyRestBinContainer();
            }

            Button {
              text: "rempty Overflow"
              onClicked: maker.emptyOverflowContainer();
            }

            Button {
              text: "Clean machine"
              onClicked: maker.cleanTheMachine();
            }

            Button {
              text: "refill 50g beans"
              onClicked: {
                maker.addBeanstoContainer(50);
              }
            }

            Button {
              text: "refill 330ml water"
              onClicked: {
                maker.addWatertoContainer(330);
              }
            }

            Button {
              text: "refill 200ml milk"
              onClicked: {
                maker.addMilkToContainer(330);
              }
            }

        }

}
