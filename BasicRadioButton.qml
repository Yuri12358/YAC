import QtQuick 2.0
import QtQuick.Controls 2.14
import QtQuick.Layouts 1.14
import QtQuick.Window 2.14

RadioButton {
    id: root
    indicator: Rectangle {
        implicitWidth: 18
        implicitHeight: 18
        x: root.leftPadding
        y: root.height / 2 - height / 2
        radius: 9
        border.color: "#000000"//control.down ? "#17a81a" : "#21be2b"

        Rectangle {
            width: 10
            height: 10
            x: 4
            y: 4
            radius: 5
            color:  "#000000"//control.down ? "#17a81a" : "#21be2b"
            visible: root.checked
        }
    }
}
