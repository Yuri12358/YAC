import QtQuick 2.11
import QtQuick.Controls.Styles 1.4

BasicButton {
//	width: Constants.entryBtnWidth
	height: Constants.entryBtnHeight
	imgWidth: Constants.entryBtnWidth

	property bool showEnhancedText: false

	property string entryName: ""
	property int entryType: -1
	property string entrySizeUC: ""
	property string entrySizeC: ""

	Text {
		anchors.fill: parent
		leftPadding: imgWidth
		text: showEnhancedText ? qsTr("name: ") + entryName + qsTr("; compressed: ") + entrySizeC + qsTr("B; uncompressed: ") + entrySizeUC + qsTr("B") : entryName
		verticalAlignment: Text.AlignVCenter
	}
}
