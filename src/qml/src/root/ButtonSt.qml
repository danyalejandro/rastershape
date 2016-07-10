import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.2

Button {
	id: bt
	iconSource: (enabled ? imgEnabled : imgDisabled)
	anchors.verticalCenter: parent.verticalCenter
	implicitHeight: 30

	property string imgEnabled
	property string imgDisabled
	property bool enabled: true

	signal clickSt

	onClicked: {
		if (enabled) {
			clickSt()
		}
	}

	MouseArea {
		anchors.fill: parent
		preventStealing: true
		hoverEnabled: true
		visible: !parent.enabled
	}
	Rectangle {
		anchors.fill: parent
		opacity: 0.4
		color: "#FFFFFF"
		visible: !parent.enabled
	}
}
