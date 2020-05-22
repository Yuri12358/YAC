import QtQuick 2.11
import QtQuick.Controls 2.4
import QtQuick.Layouts 1.3
import QtQuick.Dialogs 1.3
import "./Funcs.js" as Funcs

ApplicationWindow {
    id: rootWindow
    visible: true
    width: 640
	height: 480
	minimumWidth: 640
	minimumHeight: 480
    title: qsTr("YAC (Yet another compressor)")

	property var addToArchiveDialog: null
	property var fileInfoDialog: null
	property bool archivationActive: archPB.visible
	property var programSettingsDialog: null
	property var aboutDialog: null

	property bool anyArchiveOpened: false
	Connections {
		target: guiInteractor
		onSetFileTree: {
			anyArchiveOpened = true
		}
	}

	property var errorDialog: null
	Connections {
		target: guiInteractor
		onFireShowErrorDialog: {
			errorDialog = Funcs.createWindow("qrc:/ErrorDialog.qml", rootWindow)
			if (errorDialog != null) {
				errorDialog.errorName = error
				errorDialog.show()
			}
		}
	}

	Menu {
		id: filePopup
		property string entName: ""
		property int entType: -1
		property string sizeC: "invalid"
		property string sizeUC: "invalid"

		MenuItem {
			id: infoMenuItem
			text: qsTr("Info")
			onTriggered: {
				fileInfoDialog = Funcs.createWindow("qrc:/EntryInfoDialog.qml", rootWindow)
				if (fileInfoDialog != null) {
					fileInfoDialog.entryName = filePopup.entName
					fileInfoDialog.entryType = filePopup.entType
					fileInfoDialog.sizeComp = filePopup.sizeC
					fileInfoDialog.sizeUncomp = filePopup.sizeUC
					fileInfoDialog.show()
				}
			}
		}
	}

    FolderDialog {
		id: extractToFolderDialog
		onAccepted: {
			guiInteractor.fireExtractToFolder(folder)
		}
    }

    RowLayout {
            anchors.fill: parent
        ColumnLayout {
           id: leftControlPanel
            spacing: 0
            Layout.fillHeight: true

           BasicButton {
				id: addButton
				enabled: !archivationActive
                iconEnabled: "add.svg"
                iconDisabled: "add_disabled.svg"
				toolTipText: qsTr("Add")
                Layout.leftMargin: Constants.ltMarginLR
                Layout.rightMargin: Constants.ltMarginLR
                Layout.topMargin: Constants.ltMarginUD
                Layout.bottomMargin: Constants.ltMarginUD
                Layout.preferredWidth: width
                Layout.preferredHeight: height
                Layout.alignment: Qt.AlignCenter
				onCheckedChanged: {
					addToArchiveDialog = Funcs.createWindow("qrc:/AddToArchiveDialog.qml", rootWindow)
					if (addToArchiveDialog != null) {
						addToArchiveDialog.anyArchiveOpened = anyArchiveOpened
						addToArchiveDialog.show()
					}
				}
           }

           BasicButton {
                id: extractButton
				enabled: anyArchiveOpened && !archivationActive
				iconEnabled: "extract.svg"
                iconDisabled: "extract_disabled.svg"
				toolTipText: qsTr("Extract")
                Layout.leftMargin: Constants.ltMarginLR
                Layout.rightMargin: Constants.ltMarginLR
                Layout.topMargin: Constants.ltMarginUD
                Layout.bottomMargin: Constants.ltMarginUD
                Layout.preferredWidth: width
                Layout.preferredHeight: height
                Layout.alignment: Qt.AlignCenter
                onCheckedChanged: {
					extractToFolderDialog.open()
                }
           }

            Item {
                Layout.fillHeight: true
            }

           BasicButton {
                id: programSettingsButton
                iconEnabled: "program_settings.svg"
                iconDisabled: "program_settings_disabled.svg"
                toolTipText: qsTr("YAC settings")
                Layout.leftMargin: Constants.ltMarginLR
                Layout.rightMargin: Constants.ltMarginLR
                Layout.topMargin: Constants.ltMarginUD
                Layout.bottomMargin: Constants.ltMarginUD
                Layout.preferredWidth: width
                Layout.preferredHeight: height
				Layout.alignment: Qt.AlignCenter
				onClicked: {
					programSettingsDialog = Funcs.createWindow("qrc:/ProgramSettingsDialog.qml", rootWindow)
					if (programSettingsDialog != null) {
						programSettingsDialog.show()
					}
				}
		   }

		   BasicButton {
				id: aboutButton
				iconEnabled: "about.svg"
				toolTipText: qsTr("About")
				Layout.leftMargin: Constants.ltMarginLR
				Layout.rightMargin: Constants.ltMarginLR
				Layout.topMargin: Constants.ltMarginUD
				Layout.bottomMargin: Constants.ltMarginUD
				Layout.preferredWidth: width
				Layout.preferredHeight: height
				Layout.alignment: Qt.AlignCenter
				onClicked: {
					aboutDialog = Funcs.createWindow("qrc:/ProjectInfoDialog.qml", rootWindow)
					if (aboutDialog != null) {
						aboutDialog.show()
					}
				}
		   }

		}

		ColumnLayout {

			EntryButton {
				id: dotDotFolder
				iconEnabled: "folder.svg"
				iconDisabled: iconEnabled
				centerHorizontal: false
				marginLeft: 3
				Layout.fillWidth: true
				entryName: ".."
				entryType: 1
				entrySizeC: ""
				entrySizeUC: ""
				onDoubleClicked: {
					guiInteractor.fireGoBack()
				}
			}

			ListView {
				id: entryList
				Layout.fillWidth: true
				Layout.fillHeight: true
				model: fileModel
				onCountChanged: {
					console.log("count ", count)
				}

				delegate: EntryButton {
					id: entryBtn
					iconEnabled: model.type === 0 ? "file.svg" : "folder.svg"
					iconDisabled: iconEnabled
					centerHorizontal: false
					marginLeft: 3
					width: entryList.width
					entryName: model.name
					entryType: model.type
					entrySizeC: model.sizeComp
					entrySizeUC: model.sizeUncomp
					showEnhancedText: model.type === 0 ? guiInteractor.showEnhFileInfo : false
					onDoubleClicked: {
						if (entryType === 1) {
							guiInteractor.fireEnterFolder(entryName)
						}
					}

					MouseArea {
						id: rightButtonHandler
						anchors.fill: parent
						acceptedButtons: Qt.RightButton
						onClicked: {
							if (entryBtn.entryType === 0)
							{
								filePopup.entName = entryBtn.entryName
								filePopup.entType = entryBtn.entryType
								filePopup.sizeC = entryBtn.entrySizeC
								filePopup.sizeUC = entryBtn.entrySizeUC
								filePopup.popup()
							}
						}
					}
				}
			}
		}
    }
}
