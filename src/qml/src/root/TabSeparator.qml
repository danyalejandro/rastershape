import QtQuick 2.3

Rectangle {
	height: 30
	width: 3
	color: "#FDFDFD"
	anchors.verticalCenter: parent.verticalCenter

	Rectangle {
		x: 1
		width: 1
		height: 30

		gradient: Gradient {
			GradientStop { position: 0.0; color: "#F6F7F8" }
			GradientStop { position: 1.0; color: "#B0B6BC" }
		}
	}
}
