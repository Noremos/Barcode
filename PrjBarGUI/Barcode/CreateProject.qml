import QtQuick 2.0
import QtQuick.Controls 2.14

Popup {
    id: root
    x: Math.round((parent.width - width) / 2)
    y: Math.round((parent.height - height) / 2)

    width: 400
    height: 350
    //    onWidthChanged: Math.max(200, width)
    //    onHeightChanged: Math.max(300, height)
    modal: true
    focus: true
    closePolicy: Popup.CloseOnEscape | Popup.CloseOnPressOutsideParent
    signal done
    function crProject() {
        var retcode = backend.createProject(tf_prjPath.text, tf_name.text,
                                            tf_imgPath.text)

        switch (retcode) {
        case 0:
            imageLoaded = backend.imgLoaded
            projectOpenned = true
            root.close()
            done()
            break
        case 1:
            lb_error.text = "Проект с таким именем уже существует"
            lb_error.visible = true
            break
        case 2:
            lb_error.text = "Путь к проеткту задан некорректно"
            lb_error.visible = true
            break
        case 2:
            lb_error.text = "Путь к изображению задан некорректно"
            lb_error.visible = true
            break
        default:
            lb_error.text = "Не удалось создать проект"
            lb_error.visible = true
            break
        }
    }
    function checkDone() {
        bt_done.enabled = tf_name.text.length != 0
                && tf_prjPath.text.length != 0
        if (tf_imgPath.text.indexOf(" ") !== -1 || tf_name.text.indexOf(
                    " ") !== -1 || tf_prjPath.text.indexOf(" ") !== -1) {
            bt_done.enabled = false
            lb_error.text = "Использование пробелов не допускается"
            lb_error.visible = true
        } else
            lb_error.visible = false
    }

    property var xst: 15
    Rectangle {
        id: rectangle
        anchors.fill: parent

        //name
        Label {
            id: lb_name
            x: xst
            text: qsTr("Название проекта")
            anchors.left: parent.left
            anchors.leftMargin: xst
            anchors.top: parent.top
            anchors.topMargin: 15
        }
        TextField {
            id: tf_name
            text: qsTr("")
            anchors.left: parent.left
            anchors.leftMargin: xst
            anchors.top: lb_name.bottom
            anchors.topMargin: 5
            placeholderText: "Название проекта"
            onTextChanged: checkDone()
        }

        //*******

        //select path
        Label {
            id: lb_path
            width: 28
            height: 18
            text: qsTr("Путь")
            anchors.top: tf_name.bottom
            anchors.topMargin: 20
            anchors.left: parent.left
            anchors.leftMargin: xst
        }
        TextField {
            id: tf_prjPath
            x: xst
            height: 40
            text: bt_selectFolder.fileName
            anchors.right: bt_selectFolder.left
            anchors.rightMargin: 5
            anchors.left: parent.left
            anchors.leftMargin: xst
            anchors.top: lb_path.bottom
            anchors.topMargin: 5
            placeholderText: "Путь для сохранения проекта"
        }

        LoadButton {
            id: bt_selectFolder
            x: 345
            width: 40
            height: 40
            text: qsTr("...")
            anchors.right: parent.right
            anchors.rightMargin: 15
            anchors.top: lb_path.bottom
            anchors.topMargin: 5
            onAccepted: {
                checkDone()
            }
            selectFolder: true
        }
        //********************************

        //select img
        Label {
            id: lb_img
            width: 28
            height: 18
            text: qsTr("Изображение")
            anchors.top: tf_prjPath.bottom
            anchors.topMargin: 20
            anchors.left: parent.left
            anchors.leftMargin: xst
        }
        TextField {
            id: tf_imgPath
            x: xst
            height: 40
            text: bt_selectImg.fileName
            anchors.right: bt_selectImg.left
            anchors.rightMargin: 5
            anchors.left: parent.left
            anchors.leftMargin: xst
            anchors.top: lb_img.bottom
            anchors.topMargin: 5
            placeholderText: "Путь к изображению"
        }

        LoadButton {
            id: bt_selectImg
            width: 40
            height: 40
            text: qsTr("...")
            anchors.right: parent.right
            anchors.rightMargin: 15
            anchors.top: lb_img.bottom
            anchors.topMargin: 5
            selectFolder: false
            filter: ["Image files (*.jpg *.png *.bmp *.tif)"]
            //            onAccepted: tf_imgPath.text = bt_selectImg.fileName
        }

        //********************************
        Button {
            id: bt_cansel
            y: 190
            text: qsTr("Отмена")
            anchors.bottom: parent.bottom
            anchors.bottomMargin: 15
            anchors.left: parent.left
            anchors.leftMargin: xst
            onClicked: root.close()
        }
        Button {
            id: bt_done
            x: 283
            y: 190
            text: qsTr("Готово")
            anchors.right: parent.right
            anchors.rightMargin: 15
            anchors.bottom: parent.bottom
            anchors.bottomMargin: 15
            onClicked: crProject()
            enabled: false
            // done(tf_prjPath.text, tf_name.text, tf_imgPath.text)
        }

        Label {
            id: lb_error
            color: "#db0606"
            text: qsTr("Label")
            visible: false
            wrapMode: Text.WordWrap
            anchors.top: tf_imgPath.bottom
            anchors.topMargin: 5
            anchors.left: parent.left
            anchors.leftMargin: 15
        }
    }
}

/*##^##
Designer {
    D{i:12;anchors_x:15;anchors_y:248}
}
##^##*/

