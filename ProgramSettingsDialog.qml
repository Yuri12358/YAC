import QtQuick 2.0
import QtQuick.Controls 2.4
import QtQuick.Layouts 1.3
import QtQuick.Window 2.11

Window {
	id: rootWnd
	width: 300
	height: 200
	minimumWidth: 300
	minimumHeight: 200
	title: qsTr("Program options")

	ColumnLayout {
		anchors.fill: parent

		BasicCheckBox {
			Layout.fillWidth: true
			Layout.margins: 5
			checked: guiInteractor.showEnhFileInfo
			Layout.alignment: Qt.AlignHCenter
			text: qsTr("Show enhanced file info")
			onCheckedChanged: {
				guiInteractor.showEnhFileInfo = checked
			}
		}

		Text {
			text: qsTr("Interface language: ")
			Layout.leftMargin: 5
			Layout.rightMargin: 5
		}

		ComboBox {
			Layout.fillWidth: true
			Layout.leftMargin: 5
			Layout.rightMargin: 5
			currentIndex: guiInteractor.language
			model: ListModel {
				ListElement {
					text: qsTr("English")
				}
				ListElement {
					text: qsTr("Ukrainian")
				}
			}
			onCurrentIndexChanged: {
				guiInteractor.language = currentIndex
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
