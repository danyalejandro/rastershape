import QtQuick 2.3

Rectangle {
	width: 16
	height: 16
	radius: 8
	gradient: Gradient {
		GradientStop { position: 0.0; color: "#FFFFFF" }
		GradientStop { position: 0.5; color: "#F0F0F0" }
		GradientStop { position: 1.0; color: "#CDCDCD" }
	}
	border.width: 1
	border.color: "#808080"

	property string iconSource
	signal clicked

	Image {
		anchors.centerIn: parent
		source: parent.iconSource
	}

	MouseArea {
		anchors.fill: parent
		hoverEnabled: true
		onClicked: parent.clicked(mouse)
		onContainsMouseChanged: {
			parent.border.color = (containsMouse) ? "#B2B2B2" : "#808080"
		}
		onPressedChanged: {
			parent.border.color = (pressed) ? "#111111" : "#808080"
		}
	}
}
