import QtQuick 2.3
import QtQuick.Controls 1.2

ToolButton {
	id: bt
	iconSource: (enabled ? imgEnabled : imgDisabled)
	anchors.verticalCenter: parent.verticalCenter

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
}
