import QtQuick 2.11
import QtQuick.Controls.Styles 1.4

BasicButton {
//	width: Constants.entryBtnWidth
	height: Constants.entryBtnHeight
	imgWidth: Constants.entryBtnWidth

	property string entryName: ""
	property int entryType: -1
	property string entrySizeUC: ""
	property string entrySizeC: ""

	Text {
		anchors.fill: parent
		leftPadding: imgWidth
		text: entryName
		verticalAlignment: Text.AlignVCenter
	}
}
