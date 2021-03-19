import QtQuick 2.14
import QtQuick.Controls 2.14
import QtQuick.Layouts 1.3
import QtQuick.Dialogs 1.2
import Qt.labs.settings 1.1
import My 1.0
import QtCharts 2.3

ApplicationWindow {
    id: window
    objectName: "window"
    visible: true
    width: 1920
    height: 1080
    minimumWidth: 600
    minimumHeight: 400

    title: qsTr("Geobar")
    visibility: "Maximized"
    property int imgWid: 0
    property int imgHei: 0
    property bool projectOpenned: false
    property bool imageProcessed: false
    property bool imageCompired: false
    property bool imageLoaded: false
    property bool useOneBar: false
    property bool partsZero: true

    function clearProject() {
        mainImg.clear()
        imgTable.clearAll()
        partTable.clearAll()
        setProjectStatus(false)
    }

    function addSubImg(img_id, path) {
        console.log("addSubImg")
        imgTable.addImg(img_id, path)
    }

    function loadMainImg() {
        fileDialog.nameFilters = ["Image files (*.jpg *.png *.bmp *.tif)"]
        fileDialog.title = "Загрузить изображение"
        fileDialog.loadImg = true
        fileDialog.visible = true
    }

    function setPath(path) {
        topMenubar.zoomer.value = 100
        imgWid = scrollImg.width
        imgHei = scrollImg.height
        scrollImg.contentWidth = imgWid
        scrollImg.contentHeight = imgHei
        mainImg.setSource(path)

        backend.imgLoaded = true
        topMenubar.zoomer.setZoom(mainImg.imageWidght, mainImg.imageHeight)
    }

    function setProjectStatus(status) {
        topMenubar.setEnableStart(backend.compired)
    }

    function exit() {
        Qt.callLater(Qt.quit)
    }

    ///////////////////////////////////////////////////
    function detectOnMainImage(compirePorog, proc, type, normalize) {
        backend.findSimmularObjects(compirePorog, proc, type, normalize)
        //        findedRectsImg.clear()
        //        findedRectsImg.update()
        //        mainImg.source = "file:///" + backend.prjPath + "/marcedImg.png"
        //        findedRectsImg.source = "file:///" + backend.prjPath + "/result.png"
        setActualMainImage()
        topMenubar.setEnableStart(true)
    }

    function processImage(skipWhite, skipBlack, avgType, funtType, fone, dropFoneP) {

        backend.processMainImg(skipWhite, skipBlack, avgType, funtType, fone,
                               dropFoneP)

        mainImg.setSource("file:///" + backend.prjPath + "/marcedImg.png")
        findedRectsImg.clear()
        imageProcessed = true
        imageCompired = false
        useOneBar = backend.useOneBar
        //        topMenubar.btn_process.enabled = true
        addParts()
    }
    function retryProcess(newPorog) {
        backend.retryCompire(newPorog)
        findedRectsImg.setSource("file:///" + backend.prjPath + "/result.png")
        findedRectsImg.update()
    }

    //////////////////////////////////////////////////////
    function save() {
        backend.save()
    }
    function openProject() {
        fileDialog.nameFilters = ["Barcode Studio project (*.bc)"]
        fileDialog.selectFolder = false
        fileDialog.loadImg = false
        fileDialog.title = "Открыть проект"
        fileDialog.visible = true
    }

    function zoomImage(otn) {
        //        mainImg.setZoom(otn)
        //        findedRectsImg.setZoom(otn)
        //                mainImg.width = imgWid * otn
        //                mainImg.height = imgHei * otn
        //        mainImg.scale = otn
        scrollImg.contentWidth = Math.round(imgWid * (otn))
        scrollImg.contentHeight = Math.round(imgHei * (otn))
    }
    function addSeleted() {
        var parts = backend.getAllSelected()
        for (var i = 0; i < parts.length; ++i) {
            var id_pos = parts[i].indexOf(':')
            var img_id = parts[i].substring(0, id_pos)
            var base = parts[i].slice(id_pos + 1)
            imgTable.addImg(img_id, base)
        }
        parts.length = 0
    }

    function addParts() {
        partTable.clearAll()
        var parts = backend.getParts()
        if (parts.length === 0) {
            partsZero = true
            return
        }
        partsZero = false

        for (var i = 0; i < parts.length; ++i) {
            var id_pos = parts[i].indexOf(':')
            var img_id = parts[i].substring(0, id_pos)
            var base = parts[i].slice(id_pos + 1)
            partTable.addImg(img_id, base)
        }

        parts.length = 0
    }

    function tryProcessMainImage(skipWhite, skipBlack, avgtype, procType, fonePorog, dropFone) {
        mainImg.image = backend.tryProcessMainImage(skipWhite, skipBlack,
                                                    avgType, procType,
                                                    fonePorog, dropFone)
        findedRectsImg.clear()
        findedRectsImg.update()
    }

    function setActualMainImage() {
        if (backend.prjProcessed && backend.displayAllFinded)
            setPath("file:///" + backend.prjPath + "/marcedImg.png")
        else if (backend.imgLoaded)
            setPath("file:///" + backend.prjPath + "/source.png")
        else
            setPath("")

        if (backend.compired)
            findedRectsImg.setSource(
                        "file:///" + backend.prjPath + "/result.png")
        else
            findedRectsImg.clear()

        findedRectsImg.visible = backend.displayResults
        findedRectsImg.update()
    }
    function setPorogShoj() {
        topMenubar.setPorog()
    }

    FileDialog {
        id: fileDialog
        title: "Выберите изображение"
        property bool loadImg: true
        folder: shortcuts.home
        selectMultiple: false
        selectExisting: true

        onAccepted: if (loadImg) {
                        backend.loadMainImg(fileDialog.fileUrl.toString(
                                                ).substring(8))
                        setProjectStatus(true)
                        imageLoaded = true
                        imageCompired = false
                        imageProcessed = false
                        setActualMainImage()
                        partTable.clearAll()
                    } else {
                        var path = fileDialog.fileUrl.toString().substring(8)

                        console.log(path)
                        backend.openProject(path)
                        projectOpenned = true
                        useOneBar = backend.useOneBar
                        imageLoaded = backend.imgLoaded
                        imageProcessed = backend.prjProcessed
                        imageCompired = backend.compired

                        setPorogShoj()
                        setProjectStatus()

                        setActualMainImage()
                        imgTable.clearAll()
                        addSeleted()

                        addParts()
                    }

        onRejected: {
            console.log("Canceled")
        }
        Component.onCompleted: visible = false
    }
    FileDialog {
        id: saveDialog
        nameFilters: ["Image files (*.jpg *.png *.bmp *.tif)"]
        title: "Выберите путь для сохранения"
        //                    folder: backend.prjPath ///" //Here you can set your default folder
        selectExisting: false
        defaultSuffix: "png"
        selectMultiple: false
        property bool saveAll: false
        onAccepted: backend.exportResult(fileUrl.toString().substring(8),
                                         saveAll)
    }
    Component.onCompleted: setProjectStatus(false)
    onClosing: backend.save()
    CreateProject {
        id: createPrjID
        onDone: {
            imageLoaded = backend.imgLoaded

            projectOpenned = true
            useOneBar = backend.useOneBar
            imageLoaded = backend.imgLoaded
            imageProcessed = backend.prjProcessed
            imageCompired = backend.compired
            setActualMainImage()
            desImg.removeImg()
            partTable.clearAll()
            imgTable.clearAll()
            imgTable.removeImg()
        }
    }
    Help {
        id: help
    }

    menuBar: MenuBar {
        width: window.width
        Menu {
            title: qsTr("&Файл")
            Action {
                text: qsTr("Создать новый проект...")
                onTriggered: createPrjID.open()
            }
            Action {
                text: qsTr("Открыть проект...")
                onTriggered: openProject()
            }
            Action {
                id: ma_loadImg

                text: qsTr("Загрузить изображение...")
                enabled: projectOpenned
                onTriggered: loadMainImg()
            }
            Action {
                id: ma_save

                text: qsTr("Сохранить изменения")
                enabled: projectOpenned
                onTriggered: backend.save()
            }
            Action {
                id: ma_export

                text: qsTr("Экспортировать зелёные...")
                enabled: imageCompired
                onTriggered: {
                    saveDialog.saveAll = false

                    saveDialog.open()
                }
            }
            Action {
                id: ma_export_all

                text: qsTr("Экспортировать полностью...")
                enabled: imageCompired
                onTriggered: {
                    saveDialog.saveAll = true

                    saveDialog.open()
                }
            }
            MenuSeparator {}
            Action {
                text: qsTr("&Выйти")
                onTriggered: exit()
            }
        }
        Menu {
            title: qsTr("&Справка")
            Action {
                text: qsTr("&О программе")
                onTriggered: {
                    help.open()
                }
            }
        }
    }

    SplitView {
        id: mainSplit
        anchors.fill: parent
        orientation: Qt.Horizontal
        Rectangle {
            id: middleRect
            SplitView.minimumWidth: window.width * 0.4
            implicitWidth: window.width * 0.7
            width: window.width * 0.7
            //            SplitView.preferredWidth: window.width * 0.7
            TopMenubar {
                id: topMenubar
                width: parent.width - 10
                btn_load.onClicked: loadMainImg()
                //btn_run.onClicked: processImage()
                zoomer.onValueChanged: zoomImage(zoomer.value / 100)
                z: 2
            }
            Flickable {
                id: scrollImg
                clip: true
                anchors.right: parent.right
                anchors.rightMargin: 0
                anchors.left: parent.left
                anchors.leftMargin: 0
                anchors.bottom: parent.bottom
                anchors.bottomMargin: 10
                anchors.top: topMenubar.bottom
                anchors.topMargin: 5
                onMovementStarted: if (ma.pressed) {
                                       //                                       mainImg.setRect(scrollImg.left,
                                       //                                                       scrollImg.top,
                                       //                                                       imgWid, imgHei)
                                       //                                       findedRectsImg.setRect(scrollImg.left,
                                       //                                                              scrollImg.top,
                                       //                                                              imgWid, imgHei)
                                       ma.cursorShape = Qt.ClosedHandCursor
                                   }
                onMovementEnded: ma.cursorShape = Qt.OpenHandCursor
                ImageItem {
                    id: mainImg
                    anchors.fill: parent
                    visible: true
                    antialiasing: false
                    smooth: false

                    source: "qrc:/qtquickplugin/images/template_image.png"
                    ImageItem {
                        id: findedRectsImg
                        anchors.fill: parent
                        z: 0
                    }
                    MouseArea {
                        id: ma
                        anchors.fill: parent

                        acceptedButtons: Qt.LeftButton
                        cursorShape: Qt.OpenHandCursor
                        z: 5
                    }
                }
                ScrollBar.horizontal: ScrollBar {
                    id: hbar
                    active: vbar.active
                }
                ScrollBar.vertical: ScrollBar {
                    id: vbar
                    active: hbar.active
                }
            }
        }
        Rectangle {
            id: rightMB
            //            width: window.width * 0.3
            //            SplitView.preferredWidth: window.width * 0.3
            //            implicitWidth: window.width * 0.3
            //            SplitView.fillWidth: true
            SplitView.minimumWidth: 200
            SplitView.fillWidth: true
            color: "#adb0af"
            enabled: imageProcessed
            border.color: "#000000"
            anchors.topMargin: 0

            SplitView {
                anchors.fill: parent
                orientation: Qt.Vertical
                Keys.forwardTo: [imgTable]
                TabBar {
                    id: bar
                    width: parent.width
                    currentIndex: 0
                    TabButton {
                        text: qsTr("Выбранное изображение")
                    }
                    TabButton {
                        text: qsTr("График изображения")
                        onClicked: bargraph.setBar(desImg.imgId)
                    }
                    TabButton {
                        text: qsTr("Все выбранные изображения")
                    }
                }

                StackLayout {
                    currentIndex: bar.currentIndex
                    width: parent.width
                    implicitHeight: rightMB.height * 0.4
                    SplitView.preferredHeight: rightMB.height * 0.4
                    DesiredImg {
                        id: desImg
                    }
                    Bargraph {
                        id: bargraph
                    }
                    SelectMenu {
                        id: imgTable
                        onImgDoubleClick: {
                            bar.currentIndex = 0
                            desImg.setActiveImg(imgId)
                        }
                        onCurrentImgIdChanged: {
                            if (currentImgId === null) {
                                desImg.removeImg()
                                bargraph.removeBar()
                            } else if (!desImg.imgLoaded)
                                desImg.setActiveImg(currentImgId)
                        }
                        onRemoved: {
                            if (desImg.imgId == imgId)
                                desImg.removeImg()
                        }
                    }
                }
                Item {
                    id: partItem
                    clip: false
                    Button {
                        id: addToSubImg
                        width: parent.width * 0.5
                        text: "Добавить к набору"
                        anchors.horizontalCenter: parent.horizontalCenter
                        anchors.top: parent.top
                        anchors.topMargin: 5
                        enabled: imageProcessed && !partsZero
                        onClicked: {
                            var new_img_id = backend.addPartToSelects(
                                        partTable.currentImgId)

                            imgTable.addImg(new_img_id)
                        }
                    }

                    SubImgsTable {
                        id: partTable
                        anchors.right: parent.right
                        anchors.rightMargin: 5
                        anchors.left: parent.left
                        anchors.leftMargin: 5
                        anchors.bottom: parent.bottom
                        anchors.bottomMargin: 5
                        anchors.top: addToSubImg.bottom
                        anchors.topMargin: 3
                    }
                }
            }
        }
    }
}

/*##^##
Designer {
    D{i:19;anchors_height:0;anchors_width:0;anchors_x:0;anchors_y:0}
}
##^##*/

