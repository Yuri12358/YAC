import QtQuick 2.0
import QtQuick.Controls 2.4
import QtQuick.Layouts 1.3
import QtQuick.Window 2.11

Window {
	id: rootWnd
	width: 300
	height: 100
	minimumWidth: 300
	minimumHeight: 100
	title: qsTr("Program options")

	property bool showEnhancedFileInfo: false

	ColumnLayout {
		anchors.fill: parent

		BasicCheckBox {
			Layout.fillWidth: true
			Layout.margins: 5
			checked: showEnhancedFileInfo
			Layout.alignment: Qt.AlignHCenter
			text: qsTr("Show enhanced file info")
			onCheckedChanged: {
				guiInteractor.fireShowEnhancedFileInfo(checked)
			}
		}

		Button {
			text: qsTr("Ok")
			width: height * 3
			height: Constants.flatBtnHeight
			Layout.preferredWidth: width
			Layout.alignment: Qt.AlignHCenter
			Layout.row: 1
			Layout.column: 0
			checkable: false
			onClicked: {
				rootWnd.close()
			}
		}
	}
}
