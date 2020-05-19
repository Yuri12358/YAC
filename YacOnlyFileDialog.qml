import QtQuick 2.0
import QtQuick.Dialogs 1.3

BasicFileDialog {
    title: qsTr("Select one or more .yac files...")
    selectMultiple: true
    nameFilters: [ "YAC files (*.yac)" ]
    selectedNameFilter: nameFilters[0]
}
