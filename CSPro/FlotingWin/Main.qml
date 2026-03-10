import QtQuick 2.15
import QtQuick.Window 2.15

Window {
    id: root
    width: 25                     // 直径 25
    height: 25
    flags: Qt.FramelessWindowHint | Qt.WindowStaysOnTopHint
    color: "transparent"           // 窗口透明

    // 圆形背景
    Rectangle {
        anchors.fill: parent
        radius: width / 2           // 圆形
        color: "lightblue"          // 淡蓝色背景
        border.color: "gray"        // 可选边框
        border.width: 1

        // 中间文字 "ON"
        Text {
            anchors.centerIn: parent
            text: "ON"
            color: "black"           // 文字颜色
            font.pixelSize: 12       // 根据25x25适当调整，可改为10
            font.bold: true
        }
    }

    // 鼠标区域：处理拖动和双击
    MouseArea {
        anchors.fill: parent
        drag.target: root            // 启用简单拖动
        drag.threshold: 0             // 避免拖动阈值
        onDoubleClicked: {
            // 触发C++信号，通知主窗口显示
            floatingWindow.doubleClicked()
        }
    }
}
