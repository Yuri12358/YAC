import QtQuick 2.0
import QtQuick.Controls 2.4
import QtQuick.Layouts 1.3
import QtQuick.Window 2.11

BasicDialogWindow {
    id: rootWnd
    width: 300
	height: 200
	//minimumWidth: 300
	//minimumHeight: 200
	title: qsTr("Choose what to add or open...")

	property bool anyArchiveOpened: false
	property bool archivationActive: false

	Connections {
		target: guiInteractor
		onSetFileTree: {
			anyArchiveOpened = true
		}
	}

	Connections {
		target: guiInteractor
		onSetProgress: {
			if (progress >= 0 && progress <= 1) {
				archivationActive = true
			}
			else {
				archivationActive = false
			}
		}
	}

    MultifileDialog {
		id: addFileMD
		onAccepted: {
			guiInteractor.fireAddFilesToArchive(fileUrls)
		}
    }

    FolderDialog {
        id: addFolderFD
		onAccepted: {
			guiInteractor.fireAddFilesToArchive([folder])
		}
	}

	property string newArchName: "unnamed"
	FolderDialog {
		id: newArchiveFD
		onAccepted: {
			guiInteractor.fireNewArchive(folder, newArchName)
		}
	}

	YacOnlyFileDialog {
		id: openArchFD
		onAccepted: {
			guiInteractor.fireOpenArchive(fileUrl)
		}
	}

	ColumnLayout {
		anchors.fill: parent

    GridLayout {
        id: glRoot
        columnSpacing: 10
        rowSpacing: 10
        columns: 2
		rows: 3

		ColumnLayout {
			Layout.row: 0
			Layout.column: 0
            Layout.alignment: Qt.AlignCenter
            BasicRadioButton {
				id: rbAddFile
				enabled: anyArchiveOpened && !archivationActive
                text: qsTr("Add file")
            }
			BasicRadioButton {
				id: rbAddFolder
				enabled: anyArchiveOpened && !archivationActive
				text: qsTr("Add folder")
			}
			BasicRadioButton {
				id: rbOpenArchive
				enabled: !archivationActive
				checked: true
				text: qsTr("Open archive")
            }
			BasicRadioButton {
				id: rbNewArchive
				enabled: !archivationActive
				text: qsTr("New archive")
			}
		}

		ColumnLayout {

			Layout.fillWidth: true
			Layout.row: 0
			Layout.column: 1
			TextField {
				id: newArchTextField
				Layout.fillWidth: true
				Layout.margins: 5
				visible: rbNewArchive.checked
				onAccepted: {
					if (visible && !archivationActive) {
						newArchName = text
						newArchiveFD.open()
					}
				}
			}

			Button {
				enabled: !archivationActive
				text: rbNewArchive.checked ? qsTr("Create in...") : qsTr("Open...")
				width: height * 3
				height: Constants.flatBtnHeight
				Layout.preferredWidth: width
				Layout.alignment: Qt.AlignCenter
				checkable: false
				onClicked: {
					if (rbAddFile.checked) {
						addFileMD.open()
					}
					else if (rbAddFolder.checked) {
						addFolderFD.open()
					}
					else if (rbOpenArchive.checked){
						openArchFD.open()
					}
					else {
						newArchName = newArchTextField.text
						newArchiveFD.open()
					}
				}
			}
		}
	}
		Button {
			text: qsTr("Finish")
			width: height * 3
			height: Constants.flatBtnHeight
			Layout.preferredWidth: width
			Layout.alignment: Qt.AlignHCenter
			Layout.row: 1
				Layout.margins: 5
			Layout.column: 0
			checkable: false
			onClicked: {
				rootWnd.close()
			}
		}
	}
}
