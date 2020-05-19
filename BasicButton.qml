import QtQuick 2.11
import QtQuick.Controls 2.4
import QtQuick.Controls.Styles 1.4

Button {
    id: root
    width: Constants.controlBtnWidth
    height: Constants.controlBtnHeight
    enabled: true
    checkable: true

    property string iconEnabled
    property string iconDisabled

    property string toolTipText: ""

    property color hoverColor: Constants.btnHoverColor
    property color checkedColor: Constants.btnCheckedColor
    property color standardColor: Constants.btnStandardColor
	property color disabledColor: Constants.btnDisabledColor

	property int imgWidth: width
	property int imgHeight: height

	property bool centerHorizontal: true
	property int marginLeft: 0

	/*style: ButtonStyle {
        background: Rectangle {
            implicitWidth: root.width
            implicitHeight: root.height
            border.width: 0
            radius: Constants.controlBtnRadius
            color: root.enabled ? ((root.hovered ? root.hoverColor : (root.checked ? root.checkedColor : root.standardColor))) : root.disabledColor
/*            Behavior on color {
                ColorAnimation {
                    from:
                    easing.type: Easing.OutSine
                    duration: 300
                }
            }*/
		/*}
	}*/

	background: Rectangle {
		implicitWidth: root.width
		implicitHeight: root.height
		border.width: 0
		opacity: enabled ? 1 : 0.5
		radius: Constants.controlBtnRadius
		color: root.enabled ? ((root.hovered ? root.hoverColor : (root.checked ? root.checkedColor : root.standardColor))) : root.disabledColor
	}

    Image {
        id: btnIcon
		width: root.imgWidth
		height: root.imgHeight
        sourceSize.width: width
        sourceSize.height: height
		//anchors.centerIn: parent
		anchors.verticalCenter: root.verticalCenter
		anchors.horizontalCenter: centerHorizontal ? root.horizontalCenter : undefined
		anchors.leftMargin: centerHorizontal ? 0 : marginLeft
        fillMode: Image.Stretch
        source: Constants.iconPathPrefix + (root.enabled ? root.iconEnabled : root.iconDisabled)
    }

	ToolTip {
        visible: root.hovered && root.toolTipText != ""
		delay: 1000
        text: root.toolTipText
    }
}
