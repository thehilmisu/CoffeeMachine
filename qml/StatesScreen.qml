import QtQuick 2.12
import QtQuick.Window 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12

// With this import you have access to special types, like enums
import CoffeeMaker 1.0

Item {

    id:stScreen

    property var recipeItem;
    property var state;
    property var btnFunction;

    onRecipeItemChanged: {
        txtStates.text = "Brew your "+recipeItem.name;
        btnStates.text = "Start"
        btnFunction = "start";
        stScreen.state = "startCommandMode";
    }

    width: parent.width
    height: parent.height

    Component.onCompleted: {
        manager.oldScreenIndex = 1;//MenuScreen
        btnFunction = "start";
        stScreen.state = "startCommandMode";
    }

    Rectangle{
        anchors.fill: parent
        color:"transparent"

        Rectangle{
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.verticalCenter: parent.verticalCenter
            width: 250
            height: 250
            color:"transparent"

            Text {
                id: txtStates
                text: qsTr("text")
                font.bold: true
                font.pixelSize: 30
                color: "white"
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.top: parent.top
            }

            Button{
                id: btnStates
                text: ""
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.bottom: parent.bottom
                width: parent.width
                height: 50
                font.bold: true
                font.pixelSize: 20
                onClicked: {

                    if(btnFunction === "start"){
                        console.log("start");
                        timer.startTimer(stateMachine, 1000);
                    }
                    else if(btnFunction === "placeCup"){
                        maker.placeCup();
                        stScreen.state = "detectCup";
                    }
                    else if(btnFunction === "addBean"){
                        maker.addBeanstoContainer(50);
                    }
                    else if(btnFunction === "addWater"){
                        maker.addWatertoContainer(330);
                    }
                    else if(btnFunction === "addMilk"){
                        maker.addMilkToContainer(200);
                    }
                    else if(btnFunction === "emptyRestbin"){
                        maker.emptyRestBinContainer();
                    }
                    else if(btnFunction === "removeCup"){
                        maker.removeCup();
                        stScreen.state = "standBy";
                    }

                }
            }

        }

    }

    function recipeHasMilk(){
        if(recipeItem["milk"] === undefined){
            return false;
        }
        return true;
    }
    function hasEnoughBeans(){
        return ((maker.beansContainerLevel - recipeItem["beans_g"]) >= 0);
    }
    function hasEnoughWater(){
        return ((maker.waterContainerLevel - recipeItem["water_ml"]) >= 0);
    }
    function hasEnoughMilk(){
        return ((maker.milkContainerLevel - recipeItem["milk"]["milk_ml"]) >= 0);
    }
    function hasEnoughSpaceRestbin(){
        return ((maker.restBinLevel + recipeItem["beans_g"]) <= maker.restBinLevelMax());
    }
    function selectGrindLevel(){
        switch(recipeItem["grind_level"]){
            case "fine" : return CoffeeMaker.Fine;
            case "medium-fine" : return CoffeeMaker.MediumFine;
            case "extra-fine" : return CoffeeMaker.ExtraFine;
            case "medium" : return CoffeeMaker.Medium;
            case "medium-coarse" : return CoffeeMaker.MediumCoarse;
            case "course" : return CoffeeMaker.Course;
            case "extra-course" : return CoffeeMaker.ExtraCourse;
            default: return CoffeeMaker.Fine;
        }
    }

    Timer {
        id: timer

        // Start the timer and execute the provided callback on every X milliseconds
        function startTimer(callback, milliseconds) {
            timer.interval = milliseconds;
            timer.repeat = true;
            timer.triggered.connect(callback);
            timer.start();
        }

        // Stop the timer and unregister the callback
        function stopTimer(callback) {
            timer.stop();
            timer.triggered.disconnect(callback);
        }


    }

    ///States
    ///1- place a cup state
    ///   -> isCupPlaced?
    ///2- Check if materials are enough
    ///   -> no  -> prompt necessary actions to user (addWater, addMilk, clean, empty overflow)
    ///   -> yes -> ok
    ///3- empty bin
    ///4- grind(), brew()
    ///5- remove cup
    ///6- standby

    function stateMachine(){

        console.log(stScreen.state);
        //we assume machine is ON
        if(stScreen.state === "startCommandMode"){
            maker.startCommandMode();

            stScreen.state = "placeCup";
        }
        else if(stScreen.state === "placeCup"){
            txtStates.text = "Please place a cup";

            btnStates.text = "Place cup";

            btnFunction = "placeCup";
        }
        else if(stScreen.state === "detectCup"){
            if(maker.cupDetected){
                stScreen.state = "checkIngredients";
            }else{
                stScreen.state = "placeCup";
            }
        }
        else if(stScreen.state === "checkIngredients"){

            txtStates.text = "Checking Ingredients...";

            btnStates.visible = false;

            if(!hasEnoughBeans()){

                txtStates.text = "You need to refill beans!";

                btnStates.text = "Refill 50g beans";

                btnFunction = "addBean";

                btnStates.visible = true;
            }
            else if(!hasEnoughWater()){
                txtStates.text = "You need to refill water!";

                btnStates.text = "Refill 330ml water";

                btnFunction = "addWater";

                btnStates.visible = true;
            }
            else if(recipeHasMilk()){
                console.log(recipeItem["milk"] === undefined);
                if(!hasEnoughMilk()){
                    txtStates.text = "You need to refill milk!";

                    btnStates.text = "Refill 200ml milk";

                    btnFunction = "addMilk";

                    btnStates.visible = true;
                }else{
                    stScreen.state = "checkRestbin";
                }
            }
            else
            {
                stScreen.state = "checkRestbin";
            }
        }

        else if(stScreen.state === "checkRestbin"){
            if(!hasEnoughSpaceRestbin()){
                btnFunction = "emptyRestbin";
            }
            else{
                stScreen.state = "grindBeans";
            }
        }
        else if(stScreen.state === "grindBeans"){

            txtStates.text = "Grinding...";

            btnStates.visible = false;

            maker.doGrinding(recipeItem["beans_g"], selectGrindLevel());

            if(recipeHasMilk()){
                stScreen.state = "prepMilk";
            }else{
                stScreen.state = "doBrew";
            }
        }
        else if (stScreen.state === "prepMilk"){

            txtStates.text = "Preparing Milk...";

            maker.doMilkPrep(recipeItem["milk"]["milk_ml"],
                             recipeItem["milk"]["milk_temp"],
                             recipeItem["milk"]["foam_ml"]);

            stScreen.state = "doBrew";
        }
        else if (stScreen.state === "doBrew"){

            txtStates.text = "Brewing...";

            maker.doBrew(recipeItem["water_ml"], recipeItem["water_temp"]);

            stScreen.state = "coffeeReady";
        }

        else if(stScreen.state === "coffeeReady"){

            txtStates.text = "Your "+ recipeItem["name"] + " is ready";

            btnStates.text = "Remove your cup";

            btnStates.visible = true;

            btnFunction = "removeCup";
        }
        else if(stScreen.state === "standBy"){
            maker.finishCommandMode();

            //maker.cleanTheMachine();

            stScreen.state = "startCommandMode";

            manager.activeScreenIndex = 1;//menuScreen

            timer.stopTimer(stateMachine);

        }


    }
}
