import QtQuick 2.0
import QtQuick.Controls 2.4
import QtQuick.Layouts 1.3
import QtQuick.Window 2.11

Window {
	id: rootWnd
	width: 300
	height: 100
	minimumWidth: 300
	minimumHeight: 150
	title: entryName === "" ? qsTr("File info") : entryName + qsTr(" info")

	property string entryName: ""
	property int entryType: -1
	property string sizeComp: "invalid"
	property string sizeUncomp: "invalid"

	ColumnLayout {
		anchors.fill: parent
		Text {
			Layout.fillWidth: true
			Layout.margins: 5
			Layout.alignment: Qt.AlignHCenter
			text: (entryType === 1 ? qsTr("Folder name: ") : qsTr("File name: ")) + entryName
		}

		Text {
			Layout.fillWidth: true
			Layout.margins: 5
			Layout.alignment: Qt.AlignHCenter
			text: qsTr("Compressed size: ") + sizeComp
		}

		Text {
			Layout.fillWidth: true
			Layout.margins: 5
			Layout.alignment: Qt.AlignHCenter
			text: qsTr("Initial size: ") + sizeUncomp
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
