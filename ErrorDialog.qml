import QtQuick 2.0
import QtQuick.Controls 2.4
import QtQuick.Layouts 1.3
import QtQuick.Window 2.11

BasicDialogWindow {
	id: rootWnd
	width: 300
	height: 100
	//minimumWidth: 300
	//minimumHeight: 100
	title: qsTr("Error")

	property string errorName: qsTr("Unknown error!")

	ColumnLayout {
		anchors.fill: parent
		Text {
			Layout.fillWidth: true
			Layout.margins: 15
			Layout.alignment: Qt.AlignHCenter
			text: qsTr("Error: ") + errorName
			wrapMode: Text.WrapAnywhere
		}

		Button {
			text: qsTr("Ok")
			width: height * 3
			height: Constants.flatBtnHeight
			Layout.preferredWidth: width
			Layout.alignment: Qt.AlignHCenter
			checkable: false
			onClicked: {
				rootWnd.close()
			}
		}
	}
}
