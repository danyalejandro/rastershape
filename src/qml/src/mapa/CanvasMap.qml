import QtQuick 2.3
import "CanvasMap.js" as Log
import "qrc:/global.js" as Glo

// Canvas de dibujo
Canvas {
	id: rootCanvas
	x: 0
	y: 0
	anchors.fill: parent
	smooth: false
	visible: false

	property var ctx
	property int escala
	property int offsetX: 0 // Compensar panning con translate en canvas
	property int offsetY: 0
	property color colPincel: "#000000"
	property bool pintar: true // false es borrar
	property int tamanoLapiz: 1

	Component.onCompleted: {
		ctx = rootCanvas.getContext('2d')
		ctx.imageSmoothingEnabled = false
		ctx.save()
	}

	onVisibleChanged: {
		if (!visible) sombraMouse.visible = false
	}

	function actSombraMouse() {
		sombraMouse.actPos()
	}
	function actualizarEscala() { Log.actualizarEscala() }
	function setColor() { ctx.fillStyle = colPincel }

	function mostrarCanvas(p) {
		pintar = p
		colPincel = (p ? "#000000" : "#FFFFFF")
		setColor()
		requestPaint()
		visible = true
	}

	function ocultarCanvas() {
		visible = false
	}

	MouseArea {
		id: mArea
		anchors.fill: parent
		acceptedButtons: Qt.LeftButton | Qt.MiddleButton
		preventStealing: true
		cursorShape: Qt.CrossCursor
		hoverEnabled: true

		property int xAnt
		property int yAnt
		property int xMin
		property int yMin
		property int xMax
		property int yMax

		onPressed: Log.evMousePress(mouse.x, mouse.y)
		onReleased: Log.evMouseUp(mouse)
		onPositionChanged: Log.evMouseMove(mouse, pressed)
		onContainsMouseChanged: {
			if (!containsMouse) sombraMouse.visible = false
		}

		SombraMouse {
			id: sombraMouse
			esc: rootCanvas.escala
			visible: false
			tamano: rootCanvas.tamanoLapiz
			col: pintar ? "#444444" : "#DDDDDD"
		}
	}
}
