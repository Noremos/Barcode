import QtQuick 2.13
import QtQuick.Controls 2.13

Popup {
    id: root
    width: 500
    height: 200
    onOpened: {
        sw_displayAll.checked = backend.displayAllFinded
        sw_displayResults.checked = backend.displayResults
    }
    function updateSets() {
        backend.displayAllFinded = sw_displayAll.checked
        backend.displayResults = sw_displayResults.checked
        setActualMainImage()
    }

    Switch {
        id: sw_displayAll
        text: qsTr("Отображать все найденные объекты")
        checked: backend ? backend.displayAllFinded : true
        font.pointSize: 10
        anchors.left: parent.left
        anchors.leftMargin: 20
        anchors.top: parent.top
        anchors.topMargin: 20
        onCheckedChanged: updateSets()
    }

    Switch {
        id: sw_displayResults
        x: 20
        y: 71
        text: qsTr("Отображать найденные нужные объекты")
        checked: backend ? backend.displayResults : true
        font.pointSize: 10
        onCheckedChanged: updateSets()
    }

    Button {
        id: button
        y: 416
        text: qsTr("Закрыть")
        anchors.right: parent.right
        anchors.rightMargin: 20
        anchors.left: parent.left
        anchors.leftMargin: 20
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 20
        onClicked: {
            close()
        }
    }
}

/*##^##
Designer {
    D{i:1;anchors_x:41;anchors_y:61}D{i:3;anchors_x:25;anchors_y:416}
}
##^##*/

