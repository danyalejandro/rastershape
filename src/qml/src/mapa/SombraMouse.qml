import QtQuick 2.3

Item {
	id: rootSom
	width: esc * 3
	height: esc * 3

	property real esc
	property int tamano: 1 // 0: 1 pixel, 1: 2 pixeles
	property color col: "#AAAAAA"

	// Actualizar la posición basado en las coords del mouse
	function actPos(x, y, offsetX, offsetY) {
		var e = esc
		var ofXs = Math.abs(offsetX % e)
		var ofYs = Math.abs(offsetY % e)

		sombraMouse.x = Math.floor((x + ofXs) / e) * e - e - ofXs
		sombraMouse.y = Math.floor((y + ofYs) / e) * e - e - ofYs
	}

	Rectangle {
		color: col
		opacity: 0.9
		width: parent.esc
		height: width
		anchors.centerIn: parent
		visible: (parent.tamano == 0)
	}

	Rectangle {
		color: col
		opacity: 0.9
		width: height * 3
		height: parent.esc
		anchors.centerIn: parent
		visible: (parent.tamano == 1)
	}
	Rectangle {
		color: col
		opacity: 0.9
		width: parent.esc
		height: width * 3
		anchors.centerIn: parent
		visible: (parent.tamano == 1)
	}
}
