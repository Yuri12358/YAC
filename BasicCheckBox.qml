import QtQuick 2.0
import QtQuick.Controls 2.4
import QtQuick.Layouts 1.3
import QtQuick.Window 2.11

CheckBox {
	id: root
	indicator: Rectangle {
		implicitWidth: 18
		implicitHeight: 18
		x: root.leftPadding
		y: root.height / 2 - height / 2
		radius: 0
		border.color: "#000000"//control.down ? "#17a81a" : "#21be2b"

		Rectangle {
			width: 10
			height: 10
			x: 4
			y: 4
			radius: 0
			color:  "#000000"//control.down ? "#17a81a" : "#21be2b"
			visible: root.checked
		}
	}
}
