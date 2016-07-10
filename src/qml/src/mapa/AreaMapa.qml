import QtQuick 2.3
import "AreaMapa.js" as Logica
import "qrc:/global.js" as Glo
import "qrc:/etapas" as Etapas

// Área de imagen con pan, zoom y minimapa
Item {
	id: rootMapa
	property real escala: 1 // 1: original, factor de escala
	property int numEscala: 0 // Posición de la escala en el arreglo de escalas
	property int tamanoLapiz: 1 // 0 delgado, 1 grueso

	default property alias contents: contenido.children

	signal mapaClick(var mouse)
	signal mouseMovimiento(var mouse)

	function actualizImagen() { Logica.actualizImagen() }
	function actualizTrazos() { Logica.actualizTrazos() }
	function mostrarTrazos() { Logica.mostrarTrazos() }
	function ocultarTrazos() { Logica.ocultarTrazos() }
	function liberarTrazos() { Logica.liberarTrazos() }
	function origenTrazos(ori) { tilesTrazos.source = ori }
	function setMouseCursor(cod) { Logica.setMouseCursor(cod) }
	function getAnchoMapa() { return tilesMapa.width; }
	function getAltoMapa() { return tilesMapa.height; }

	function ajustarMinimapa() { miniMapa.reajustar() }

	// zoom
	function zoomIn() { Logica.zoomIn() }
	function zoomOut() { Logica.zoomOut() }
	function posInit() { Logica.posInit() }
	function zoomInit() { Logica.zoomInit() }
	function zoomPanReset() { Logica.zoomPanReset() }

	// dibujo
	function mostrarCanvas(pintar) { canvasMapa.mostrarCanvas(pintar) }
	function ocultarCanvas() { canvasMapa.ocultarCanvas() }



	// Area arrastable
	Flickable {
		id: flick
		anchors.fill: parent
		contentWidth: tilesMapa.width
		contentHeight: tilesMapa.height
		clip: true
		boundsBehavior: Flickable.StopAtBounds

		property int cursorAnt

		onContentXChanged: miniMapa.actPosX()
		onContentYChanged: miniMapa.actPosY()

		onMovementStarted: {
			cursorAnt = areaMouse.cursorShape
			areaMouse.cursorShape = Qt.ClosedHandCursor
			if (canvasMapa.visible) canvasMapa.visible = false
		}
		onMovementEnded: {
			areaMouse.cursorShape = cursorAnt
			if (root.actividad == 3 || root.actividad == 4) {
				canvasMapa.actualizarEscala()
				canvasMapa.visible = true
			}
		}



		// Imagen del mapa
		Tiles {
			id: tilesMapa
			source: "bin"
		}
		Tiles {
			id: tilesTrazos
			source: "trazos"
		}

		MouseArea {
			id: areaMouse
			anchors.fill: parent
			acceptedButtons: Qt.LeftButton | Qt.RightButton | Qt.MiddleButton
			hoverEnabled: true

			onClicked: {
				if (mouse.button === Qt.LeftButton) {
					root.mapaClick(mouse)
				}
			}

			onWheel: {
				if (wheel.angleDelta.y < 0) Logica.areaZoom(-1, wheel.x, wheel.y)
				else if (wheel.angleDelta.y > 0) Logica.areaZoom(1, wheel.x, wheel.y)
			}

			onPositionChanged: rootMapa.mouseMovimiento(mouse)

		}

		Item {
			id: contenido
			anchors.fill: parent
		}
	}

	// Canvas de dibujo
	CanvasMap {
		id: canvasMapa
		escala: rootMapa.escala
		tamanoLapiz: rootMapa.tamanoLapiz
	}

	// Minimapa
	MiniMapa {
		id: miniMapa
		width: 100
		height: 70
		visible: false
		anchors.right: parent.right
		anchors.bottom: parent.bottom
		anchors.margins: 20
	}
}
