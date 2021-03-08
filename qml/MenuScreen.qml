import QtQuick 2.12
import QtQuick.Window 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12


Item {

    width: parent.width
    height: parent.height

    Component.onCompleted: {
        manager.oldScreenIndex = 0;//to the StandbyScreen
    }

    function getCoffeeList() {
        var coffeObjList = JSON.parse(coffee.coffeeList);
        return coffeObjList["recipes"];
    }

    function recipeItemSelected(item){
        console.log(item.name);
        statesScreen.recipeItem = item;
        manager.activeScreenIndex = 3; //StatesScreen
    }

    Rectangle{
        anchors.fill: parent
        color:"transparent"

        GridView {
            width:750
            height: 400
            y:5
            cellWidth: 250
            cellHeight: 240
            anchors.horizontalCenter: parent.horizontalCenter
            id:recipeList
            model: getCoffeeList()
            delegate: Column {
                Rectangle{
                    id:wrapper
                    focus:true
                    height: recipeList.cellHeight - 15
                    width: recipeList.cellWidth
                    color: "transparent"
                    Image { source: "images/Coffee.png"; anchors.horizontalCenter: parent.horizontalCenter }
                    Text {
                        text: recipeList.model[index]["name"]
                        anchors.horizontalCenter: parent.horizontalCenter
                        anchors.verticalCenter: parent.bottom
                        color: "white"
                        font.pointSize: 15
                        font.bold: true
                    }
                    MouseArea {
                        id: clickable
                        anchors.fill: wrapper
                        hoverEnabled: true
                        onClicked: {
                            //recipeList.currentIndex=index;
                            recipeItemSelected(recipeList.model[index]);
                        }

                    }
                }
            }


        }

    }
}
