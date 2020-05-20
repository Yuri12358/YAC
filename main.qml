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

	Menu {
		id: filePopup
		property string entName: ""
		property int entType: -1
		property string sizeC: "invalid"
		property string sizeUC: "invalid"

		MenuItem {
			text: qsTr("Remove")
			onTriggered: {
				console.log("Rm")
			}
		}

		MenuItem {
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
        id: extractToFolderFD
    }

    YacOnlyFileDialog {
        id: concatFD
    }

    RowLayout {
            anchors.fill: parent
        ColumnLayout {
           id: leftControlPanel
            spacing: 0
            Layout.fillHeight: true

           BasicButton {
                id: addButton
                iconEnabled: "add.svg"
                iconDisabled: "add_disabled.svg"
                toolTipText: qsTr("add")
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
                        addToArchiveDialog.show()
                    }
                }
           }

           BasicButton {
                id: extractButton
                iconEnabled: "extract.svg"
                iconDisabled: "extract_disabled.svg"
                toolTipText: qsTr("extract")
                Layout.leftMargin: Constants.ltMarginLR
                Layout.rightMargin: Constants.ltMarginLR
                Layout.topMargin: Constants.ltMarginUD
                Layout.bottomMargin: Constants.ltMarginUD
                Layout.preferredWidth: width
                Layout.preferredHeight: height
                Layout.alignment: Qt.AlignCenter
                onCheckedChanged: {
                    extractToFolderFD.open()
                }
           }

           BasicButton {
                id: concatButton
                iconEnabled: "concat.svg"
                iconDisabled: "concat_disabled.svg"
                toolTipText: qsTr("concat")
                Layout.leftMargin: Constants.ltMarginLR
                Layout.rightMargin: Constants.ltMarginLR
                Layout.topMargin: Constants.ltMarginUD
                Layout.bottomMargin: Constants.ltMarginUD
                Layout.preferredWidth: width
                Layout.preferredHeight: height
                Layout.alignment: Qt.AlignCenter
                onCheckedChanged: {
                    concatFD.open()
                }
           }

           BasicButton {
                id: settingsButton
                iconEnabled: "settings.svg"
                iconDisabled: "settings_disabled.svg"
                toolTipText: qsTr("archive settings")
                Layout.leftMargin: Constants.ltMarginLR
                Layout.rightMargin: Constants.ltMarginLR
                Layout.topMargin: Constants.ltMarginUD
                Layout.bottomMargin: Constants.ltMarginUD
                Layout.preferredWidth: width
                Layout.preferredHeight: height
                Layout.alignment: Qt.AlignCenter
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
           }
		}

		ColumnLayout {
/*		ScrollView {
			id: archivedFilesSV
        //anchors.fill: parent
			width: 100*/

			RowLayout {
				id: archProgress
				visible: false
				Layout.fillWidth: true
				ProgressBar {
					id: archPB
					indeterminate: false
					Layout.fillWidth: true
				}

				Button {
					text: qsTr("Cancel")
					width: height * 3
					height: Constants.flatBtnHeight
					Layout.preferredWidth: width
					Layout.alignment: Qt.AlignCenter
					checkable: false
					onClicked: {
						archProgress.visible = false
						guiInteractor.fireCancelCurrentArchivation();
					}
				}
				Connections {
					target: guiInteractor
					onSetProgress: {
						if (progress < 0 || progress > 1) {
							archProgress.visible = false
						}
						else {
							if (!archProgress.visible) {
								archProgress.visible = true
							}
							archPB.value = progress
						}
					}
				}
			}

			EntryButton {
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
				id: entryLV
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
					width: entryLV.width
					entryName: model.name
					entryType: model.type
					entrySizeC: model.sizeComp
					entrySizeUC: model.sizeUncomp
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
