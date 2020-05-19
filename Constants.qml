pragma Singleton
import QtQuick 2.9

QtObject {
    readonly property color btnHoverColor: "#a0f0f8ff"
    readonly property color btnCheckedColor: "#00000000"//"#d0f0f8ff"
    readonly property color btnStandardColor: "#00000000"
    readonly property color btnDisabledColor: "#00000000"

    readonly property string iconPathPrefix: "qrc:/icons/"

    readonly property int controlBtnWidth: 64
    readonly property int controlBtnHeight: controlBtnWidth
    readonly property int controlBtnRadius: 5
    readonly property int controlIconWidth: 32
    readonly property int controlIconHeight: controlIconWidth

	readonly property int flatBtnHeight: 38

	readonly property int entryBtnWidth: 32
	readonly property int entryBtnHeight: entryBtnWidth
	readonly property int entryHeight: 38

    readonly property int basicDialogWidth: 400
    readonly property int basicDialogHeight: 300

    readonly property int ltMarginLR: 8
    readonly property int ltMarginUD: 4
}
