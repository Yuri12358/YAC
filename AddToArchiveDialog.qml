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
	title: qsTr("Choose what to add or open...")

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
			guiInteractor.fireAddFilesToArchive([fileUrl])
		}
	}

    GridLayout {
        id: glRoot
        anchors.fill: parent
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
                checked: true
                text: qsTr("Add file")
            }
			BasicRadioButton {
				id: rbAddFolder
				text: qsTr("Add folder")
			}
			BasicRadioButton {
				id: rbOpenArchive
				text: qsTr("Open archive")
            }
			BasicRadioButton {
				id: rbNewArchive
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
				//placeholder: qsTr("Enter name")
				onAccepted: {
					if (visible) {
						newArchName = text
						newArchiveFD.open()
					}
				}
			}

			Button {
				text: qsTr("Open...")
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

        Button {
            text: qsTr("Ok")
            width: height * 3
            height: Constants.flatBtnHeight
            Layout.preferredWidth: width
			Layout.alignment: Qt.AlignLeft
            Layout.row: 1
				Layout.margins: 5
			Layout.column: 0
            checkable: false
            onClicked: {
                rootWnd.close()
            }
		}

        Button {
            text: qsTr("Cancel")
            width: height * 3
            height: Constants.flatBtnHeight
            Layout.preferredWidth: width
			Layout.alignment: Qt.AlignRight
			Layout.row: 1
            Layout.column: 1
				Layout.margins: 5
			checkable: false
            onClicked: {
                rootWnd.close()
            }
        }
    }
}
