import QtQuick 2.13
import QtQuick.Controls 2.13
import MyEnums 1.0

Item {
    id: root

    property bool enableUpdate: true
    property bool imgLoaded: false
    function setActiveImg(img_id) {
        var info = (backend.getImgInfo(img_id))
        if (info.mid === undefined)
            return
        root.imgId = Math.round(img_id)
        enableUpdate = false
        slImgMid.value = info.mid
        slRangeSelect.first.value = info.mid - info.foneStart
        slRangeSelect.second.value = info.mid + info.foneEnd

        chbAutodetectAvg.checked = info.isAuto
        var val = cbbAvgType.indexOfValue(info.funcID)
        cbbAvgType.currentIndex = Math.max(val, 0)
        //delete info
        enableUpdate = true
        imgLoaded = true
        //mImg.source = backend.getImg(imgId, true)
        setsChanged()
    }
    function getProcessType() {
        return chbAutodetectAvg.checked ? cbbAvgType.currentValue : ProcessType.Manuly
    }

    function setsChanged() {
        if (enableUpdate && imgLoaded)
            mImg.source = backend.processImg(imgId, getProcessType(),
                                             slRangeSelect.fv, slImgMid.value,
                                             slRangeSelect.sv,
                                             chbAutodetectAvg.checked)
    }
    function removeImg() {
        mImg.source = ""
        imgLoaded = fasle
        imgId = 0
    }

    property int imgId: 0
    Rectangle {
        id: mImgBack
        width: root.width
        enabled: false
        smooth: false
        anchors.bottom: sets0.top
        anchors.bottomMargin: 5
        anchors.top: parent.top
        anchors.topMargin: 5

        color: "green"

        Image {
            id: mImg
            layer.enabled: true
            visible: true
            enabled: false
            smooth: false
            z: 1

            anchors.fill: parent
            //            source: "qrc:/../test/loaded/-22_копия.png"
            fillMode: Image.PreserveAspectFit
        }
    }
    Row {
        id: sets0
        enabled: useOneBar
        width: root.width
        height: 45
        anchors.bottom: sets1.top
        anchors.bottomMargin: 0
        CheckBox {
            id: chbAutodetectAvg
            height: sets0.height
            width: root.width * 0.2
            text: qsTr("Autodetect")
            tristate: false
            checked: false
            display: AbstractButton.TextBesideIcon
            onCheckStateChanged: {
                slImgMid.enabled = !checked
                cbbAvgType.enabled = checked
                //setsChanged()
                var ids = getProcessType()
                var nid = backend.getImgAvg(imgId, ids)
                slImgMid.value = nid
                //slRangeSelect.first.value = slImgMid.value - slRangeSelect.fv
                //slRangeSelect.second.value = slImgMid.value - slRangeSelect.sv
            }
        }
        ComboBox {
            id: cbbAvgType
            textRole: "text"
            valueRole: "value"
            width: root.width * 0.25
            height: sets0.height
            enabled: false
            model: [{
                    "text": "По гистограмме",
                    "value": ProcessType.Hist
                }, {
                    "text": "По-среднему",
                    "value": ProcessType.AvgNsum
                }, {
                    "text": "По гистограмме снимка",
                    "value": ProcessType.HistFromMI
                }, {
                    "text": "По-среднему снимка",
                    "value": ProcessType.AvgNsumFromMI
                }]
            onCurrentIndexChanged: {
                var ids = getProcessType()
                var nid = backend.getImgAvg(imgId, ids)
                slImgMid.value = nid
            }
        }
        Label {
            text: slImgMid.value + " (-" + (slImgMid.value - slRangeSelect.first.value)
                  + "/+" + (slRangeSelect.second.value - slImgMid.value) + ")"
            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignHCenter
            width: root.width - cbbAvgType.width - chbAutodetectAvg.width
            height: root.height * 0.1
        }
    }
    Row {
        id: sets1
        enabled: useOneBar

        width: root.width
        height: 45
        anchors.bottom: sets2.top
        anchors.bottomMargin: 0
        Slider {
            id: slImgMid
            width: sets1.width
            height: sets1.height
            snapMode: Slider.SnapAlways
            stepSize: 1
            value: 128
            to: 255
            onValueChanged: {
                slRangeSelect.callChangeEv = false
                slRangeSelect.second.value = slImgMid.value + slRangeSelect.sv
                slRangeSelect.first.value = slImgMid.value - slRangeSelect.fv
                slRangeSelect.second.value = slImgMid.value + slRangeSelect.sv
                slRangeSelect.first.value = slImgMid.value - slRangeSelect.fv

                setsChanged()
                slRangeSelect.callChangeEv = true
            }
        }
    }
    Row {
        id: sets2
        enabled: useOneBar

        height: 45
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 10
        RangeSlider {
            id: slRangeSelect
            width: slImgMid.width
            height: sets0.height
            orientation: RangeSlider.SnapAlways
            stepSize: 1
            first.value: slImgMid.value - fv
            to: 255
            second.value: slImgMid.value + sv
            property int fv: 20
            property int sv: 20
            property int dif: slImgMid.value - slImgMid.value
            property bool callChangeEv: true
            first.onValueChanged: {
                if (callChangeEv) {
                    fv = Math.max(0, slImgMid.value - slRangeSelect.first.value)
                    if (fv == 0)
                        first.value = slImgMid.value
                    else
                        first.value = Math.round(first.value)
                    setsChanged()
                }
            }
            second.onValueChanged: {
                if (callChangeEv) {
                    sv = Math.max(0,
                                  slRangeSelect.second.value - slImgMid.value)

                    if (sv == 0)
                        second.value = slImgMid.value
                    else
                        second.value = Math.round(second.value)
                    setsChanged()
                }
            }
        }
    }
}

/*##^##
Designer {
    D{i:0;autoSize:true;height:480;width:640}D{i:3;anchors_height:45;anchors_width:640}
}
##^##*/

