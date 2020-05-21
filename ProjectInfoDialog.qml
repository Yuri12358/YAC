import QtQuick 2.0
import QtQuick.Window 2.11
import QtQuick.Layouts 1.3

BasicDialogWindow {
	id: rootWnd
	width: 280
	height: 200
	//minimumWidth: 300
	//minimumHeight: 150
	title: qsTr("Project info")

	ColumnLayout {
		anchors.fill: parent
		Text {
			Layout.fillWidth: true
			Layout.margins: 5
			Layout.alignment: Qt.AlignHCenter
			text: qsTr("Project YAC (yet another compressor)")
		}

		Text {
			Layout.fillWidth: true
			Layout.margins: 5
			Layout.alignment: Qt.AlignHCenter
			text: qsTr("Authors: Danylo Timoshevsky and Yuri Kovalenko")
		}

		RowLayout {
			Layout.fillHeight: true
			Layout.fillWidth: true
			Image {
				width: 100
				height: 100
				Layout.leftMargin: 10
				Layout.rightMargin: 10
				Layout.alignment: Qt.AlignLeft
				fillMode: Image.Stretch
				source: Constants.iconPathPrefix + "dt.png"
			}
			Image {
				width: 100
				height: 100
				Layout.leftMargin: 10
				Layout.rightMargin: 10
				Layout.alignment: Qt.AlignRight
				fillMode: Image.Stretch
				source: Constants.iconPathPrefix + "yk.png"
			}
		}
	}
}
