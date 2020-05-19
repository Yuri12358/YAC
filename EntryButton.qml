import QtQuick 2.12
import QtQuick.Controls 1.4 as QQC1
import QtQuick.Controls 2.12 as QQC2
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
