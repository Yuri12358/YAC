import QtQuick 2.0
import QtQuick.Dialogs 1.3

FileDialog {
    sidebarVisible: true
    selectExisting: true
    folder: shortcuts.documents
    modality: Qt.WindowModal
}
