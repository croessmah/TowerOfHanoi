import QtQuick 2.15
import QtQuick.Window 2.15

import QtQuick.Controls 2.15

Window {
    id: root

    width: 640
    height: 480
    visible: true
    title: qsTr("Tower of Hanoi")

    readonly property int ringWidth: 200
    readonly property int ringHeight: 20
    readonly property int ringDiffPx: 6
    readonly property int maxRings: 20
    readonly property int minRings: 4
    property bool running: false
    readonly property bool finished: !(hanoi.hasNext && hanoi.hasPrev)

    minimumWidth: footer.x + footer.width
    minimumHeight: footer.y + footer.height
    maximumWidth: minimumWidth
    maximumHeight: minimumHeight

    onFinishedChanged:
    {
        if (running && finished)
            running = false
    }

    Item {
        id: hanoiItem

        Connections
        {
            property var ringsColumns: [ fromItem, bufferItem, toItem ]
            target: hanoi
            ignoreUnknownSignals: true
            function onRefreshed()
            {
                moveAnimation.stopMove()
                ringsRepeater.model = hanoi.ringsCount

                const fromModel = hanoi.from()
                const toModel = hanoi.to()
                const bufferModel = hanoi.buffer()

                for(let c = 0; c < ringsColumns.length; ++c)
                    ringsColumns[c].rings = []

                for (let fI = 0; fI < fromModel.length; ++fI)
                    fromItem.push(ringsRepeater.itemAt(fromModel[fI]))

                for (let tI = 0; tI < toModel.length; ++tI)
                    toItem.push(ringsRepeater.itemAt(toModel[tI]))

                for (let bI = 0; bI < bufferModel.length; ++bI)
                    bufferItem.push(ringsRepeater.itemAt(bufferModel[bI]))
            }

            function onStepped(from, to)
            {
                if (from === to)
                    return

                ringsContainer.from_to(ringsColumns[from], ringsColumns[to])
            }
        }
    }

    Item
    {
        id: parentContainer

        width: ringsContainer.width
        height: 20 * 22

        Rectangle
        {
            anchors.fill: parent
            color: "lightgray"
        }

        Item
        {
            Repeater
            {
                id: ringsRepeater
                delegate: Item {
                    width: ringWidth
                    height: ringHeight

                    Rectangle {
                        width: parent.width - (ringsRepeater.count - index - 1) * ringDiffPx;
                        height: parent.height
                        anchors.centerIn: parent
                        color: Qt.rgba(Math.random(), Math.random(), Math.random(), 0.5)
                        radius: 4
                        border.width: 1
                        border.color: "black"
                    }

                }
            }
        }

        Row
        {
            id: ringsContainer

            height: parent.height
            spacing: 12

            function from_to(from, to)
            {
                moveAnimation.startMove(from, to)
            }

            Item {
                id: fromItem
                property var rings: []
                readonly property int index: 0
                width: ringWidth
                height: 1

                function push(ringItem)
                {
                    rings.push(ringItem)
                    ringItem.x = x
                    ringItem.y = ringsContainer.height - rings.length * ringItem.height
                }
            }
            Item {
                id: bufferItem
                property var rings: []
                readonly property int index: 1
                width: ringWidth
                height: 1
                function push(ringItem)
                {
                    rings.push(ringItem)
                    ringItem.x = x
                    ringItem.y = ringsContainer.height - rings.length * ringItem.height
                }
            }
            Item {
                id: toItem
                property var rings: []
                readonly property int index: 2
                width: ringWidth
                height: 1
                function push(ringItem)
                {
                    rings.push(ringItem)
                    ringItem.x = x
                    ringItem.y = ringsContainer.height - rings.length * ringItem.height
                }
            }
        }
    }

    Item {
        id: footer
        anchors
        {
            right: parentContainer.right
            left: parentContainer.left
            top:parentContainer.bottom
            bottomMargin: 12
        }
        height: 54

        Row
        {
            spacing: 4
            height: parent.height - 8
            anchors.centerIn: parent

            RoundButton
            {
                text: "<<"
                radius: parent.height
                enabled: hanoi.hasPrev
                onClicked:
                {
                    running = false
                    hanoi.movePrev()
                }

            }

            SpinBox
            {
                id: ringsCount
                width: 120
                height: parent.height
                from: minRings
                to: maxRings
                onValueModified:
                {
                    moveAnimation.stopMove();
                    running = false
                    hanoi.refresh(value)
                }
            }

            Button
            {
                text: running ? "pause" : "start"
                width: 48
                height: parent.height
                onClicked: {
                    if (running) {
                        moveAnimation.stopMove();
                        running = false
                    }
                    else {
                        running = true
                        if (finished)
                            hanoi.refresh(ringsCount.value)
                        hanoi.moveNext()
                    }
                }
            }
            RoundButton
            {
                text: ">>"
                radius: parent.height
                enabled: hanoi.hasNext
                onClicked:
                {
                    running = false
                    hanoi.moveNext()
                }
            }

            Dial
            {
                id: animatioSpeed
                width: parent.height
                height: parent.height
                from: 4
                to: 1000
                value: 400
                live: true
            }

            Slider
            {
                width: 120
                height: parent.height
                from: 0
                to: hanoi.stepsCount > 0 ? hanoi.stepsCount : 0
                stepSize: 1
                live: true
                value: hanoi.currentStep
                snapMode: Slider.SnapOnRelease
                handle: Item {}
                onMoved:
                {
                    running = false
                    hanoi.moveToStep(value)
                }
            }

            ProgressBar
            {
                width: 120
                height: parent.height
                from: 0
                to: 1
                value: hanoi.stepsCount > 0 ? hanoi.currentStep / hanoi.stepsCount : 0
            }

        }
    }

    SequentialAnimation
    {
        id: moveAnimation
        property Item movedItem
        property int maxY: 0
        property int moveToX: 0
        property int moveToY: 0
        property real duration: animatioSpeed.value

        function stopMove()
        {
            if (running)
            {
                stop()
                movedItem.x = moveToX
                movedItem.y = moveToY
            }
        }

        function startMove(from, to)
        {
            stopMove()

            var ringItem = from.rings.pop()
            const diffColumns = Math.abs(from.index - to.index)

            let maxCount = Math.max(from.rings.length, to.rings.length)
            if (diffColumns > 1)
            {
                maxCount = Math.max(maxCount, bufferItem.rings.length)
            }

            to.rings.push(ringItem)
            movedItem = ringItem
            maxY = ringsContainer.height - (maxCount + 2) * ringItem.height
            moveToX = to.x
            moveToY = ringsContainer.height - to.rings.length * ringItem.height

            start()
        }

        NumberAnimation {
            target: moveAnimation.movedItem
            to: moveAnimation.maxY
            property: "y"
            duration: moveAnimation.duration / 4
            easing.type: Easing.Linear
        }

        NumberAnimation {
            target: moveAnimation.movedItem
            to: moveAnimation.moveToX
            property: "x"
            duration: moveAnimation.duration / 4
            easing.type: Easing.Linear
        }

        NumberAnimation {
            target: moveAnimation.movedItem
            to: moveAnimation.moveToY
            property: "y"
            duration: moveAnimation.duration / 4
            easing.type: Easing.Linear
        }

        PauseAnimation {
            duration: moveAnimation.duration / 4
        }

        onFinished:
        {
            if (root.running)
                hanoi.moveNext()
        }
    }

    Component.onCompleted:
    {
        hanoi.refresh(minRings)
    }
}
