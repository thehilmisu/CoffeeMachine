import QtQuick 2.12
import QtQuick.Window 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12

Window {
    id: mainWindow
    width: 800; minimumWidth: 800
    height: 540; minimumHeight: 540

    title: "My Coffee Machine UI"
    visible: true


    WindowManager{
        activeScreenIndex: 0

    }
}
