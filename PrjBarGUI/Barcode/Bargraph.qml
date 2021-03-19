import QtQuick 2.14
import QtCharts 2.14
import QtQuick.Controls 2.13

Item {
    id: mRoot
    property int curImgId: 0
    function setBar(barId) {

        curImgId = barId

        function fillSeries(ind, bar) {

            var line = root.series(ind)
            line.clear()
            if (barId === 0 || bar === undefined)
                return

            var mmax = 0
            for (var el in bar) {
                const po = bar[el]
                line.append(po.x, po.y)
                mmax = Math.max(mmax, po.y)
            }

            line.axisX.min = 0
            line.axisX.max = 255

            line.axisY.min = 0
            line.axisY.max = mmax
        }
        var barObj = backend.getImgBar(barId, slider.value)
        for (var i = 0; i < 6; ++i)
            fillSeries(i, barObj[i.toString()])
    }
    function removeBar() {
        for (var i = 0; i < 6; ++i)
            root.series(i).clear()
    }
    ChartView {
        id: root
        anchors.bottom: slider.top
        anchors.right: parent.right
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.bottomMargin: 0
        title: "Line"

        antialiasing: true
        //        theme: ChartView.ChartThemeBrownSand
        LineSeries {
            name: "bar1"

            //            type: ChartView.SeriesTypeLine
        }
        LineSeries {
            name: "bar2"

            //            type: ChartView.SeriesTypeLine
        }
        LineSeries {
            name: "bar3"

            //            type: ChartView.SeriesTypeLine
        }
        LineSeries {
            name: "bar4"

            //            type: ChartView.SeriesTypeLine
        }
        LineSeries {
            name: "bar5"

            //            type: ChartView.SeriesTypeLine
        }
        LineSeries {
            name: "bar6"

            //            type: ChartView.SeriesTypeLine
        }
    }

    Label {
        id: label
        width: 70
        text: slider.value
        anchors.verticalCenter: slider.verticalCenter
        anchors.right: parent.right
        anchors.rightMargin: 0
        font.pointSize: 12
    }

    Slider {
        id: slider
        stepSize: 1
        anchors.left: parent.left
        anchors.leftMargin: 0
        anchors.right: label.left
        anchors.rightMargin: 0
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 0
        value: 0
        to: 255

        from: 0
        onValueChanged: setBar(curImgId)
    }
}
/*##^##
Designer {
    D{i:0;autoSize:true;height:480;width:640}
}
##^##*/

