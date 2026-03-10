import QtQuick 2.15
import QtQuick.Window 2.15

Window {
    id: root
    width: 80
    height: 80
    visible: false
    flags: Qt.FramelessWindowHint | Qt.WindowStaysOnTopHint | Qt.Tool
    color: "transparent"

    Rectangle {
        anchors.fill: parent
        radius: width / 2
        color: "#80C0C0C0"
        border.color: "#888"
        border.width: 2

        Text {
            anchors.centerIn: parent
            text: "●"
            font.pixelSize: 40
            color: "white"
        }

        MouseArea {
            anchors.fill: parent
            property point clickPos: "0,0"

            onPressed: {
                clickPos = Qt.point(mouseX, mouseY)
            }
            onPositionChanged: {
                const delta = Qt.point(mouseX - clickPos.x, mouseY - clickPos.y)
                root.x += delta.x
                root.y += delta.y
            }
            onClicked: {
                root.restoreMainWindow()
            }
        }
    }

    signal restoreMainWindow()
}
