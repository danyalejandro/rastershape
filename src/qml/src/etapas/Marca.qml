import QtQuick 2.3
import QtQuick.Controls 1.2

Image {
	id: rootMarca
	source: "qrc:/img/marca.png"
	width: 25; height: 26
	visible: false
	smooth: false
	antialiasing: false

	property int num: 0 // identificador [0..3]
	property real xIni: 0
	property real yIni: 0
	property real escala: 0
	property real este: txtEste.text
	property real norte: txtNorte.text
	property bool tieneDatos: (txtEste.text != "0" && txtNorte.text != "0")

	// posiciona la marca en el mapa y almacena la coordenada real
	function ubicar(posX, posY, esc) {
		x = posX - 12
		y = posY - 13
		actCoordsReales()
		visible = true

		console.log("marca ubicada: " + xIni + "," + yIni + ", escala: " + escala)
	}

	// Actualiza el valor de las variables xIni y yIni (coords 1:1)
	function actCoordsReales() {
		xIni = (x + 12) / escala
		yIni = (y + 13) / escala
		miProxy.setPosMarca(num, xIni, yIni)
	}

	// Oculta la marca y la ubica en 0,0
	function reset() {
		x = 0
		y = 0
		visible = false
	}

	// Actualiza la posición de la marca según una escala
	function actPos(escala, escalaAnt) {
		x = xIni * escala - 12
		y = yIni * escala - 13

		if (x < 67) coords.visible = false // ocultar diálogo si se va a cortar
	}

	// Obtiene la posición del centro de la marca
	function centroX() {
		return x + 12
	}
	function centroY() {
		return y + 13
	}

	// Retorna la posición real de la marca en el mapa (en escala 1:1)
	function realX() { return xIni }
	function realY() { return yIni }

	// Diálogo de coordenadas
	Image {
		id: coords
		source: "qrc:/img/tooltip-coords.png"
		anchors.horizontalCenter: parent.horizontalCenter
		y: 25
		smooth: false

		// Usada para evitar clicks dentro del diálogo emergente
		MouseArea { anchors.fill: parent }

		Label {
			id: lblEste
			x: 23
			y: 17
			text: "Este:"
		}
		TextField {
			id: txtEste
			x: 55
			y: 15
			implicitWidth: 86
			height: 18
			text: "0"
			validator: IntValidator { }
			onTextChanged: {
				if (text !== "0") {
					miProxy.setCoordsMarca(rootMarca.num, Number(txtEste.text), Number(txtNorte.text))
				}
			}
		}

		Label {
			id: lblNorte
			x: 16
			y: 40
			text: "Norte:"
		}
		TextField {
			id: txtNorte
			x: 55
			y: 38
			implicitWidth: 86
			height: 18
			text: "0"
			validator: IntValidator { }
			onTextChanged: {
				if (text !== "0") {
					miProxy.setCoordsMarca(rootMarca.num, Number(txtEste.text), Number(txtNorte.text))
				}
			}
		}

		Image {
			x: parent.width - 12
			y: -4
			source: "qrc:/img/close-button.gif"
			smooth: false

			Image {
				id: btCloseHover
				source: "qrc:/img/close-button2.png"
				visible: btCloseMouse.containsMouse
				smooth: false
			}

			MouseArea {
				id: btCloseMouse
				hoverEnabled: true
				anchors.fill: parent
				onClicked: coords.visible = false
			}
		}

	}

	Image {
		source: "qrc:/img/marca3.png"
		visible: mAreaMarca.containsMouse || coords.visible
		smooth: false
	}

	MouseArea {
		id: mAreaMarca
		anchors.fill: parent
		drag.target: parent
		drag.minimumX: -14
		drag.minimumY: -14
		drag.maximumX: parent.parent.width - 14
		drag.maximumY: parent.parent.height - 14
		cursorShape: Qt.PointingHandCursor
		hoverEnabled: true
		drag.threshold: 3

		onClicked: {
			coords.visible = !coords.visible
		}

		onReleased: {
			parent.actCoordsReales()
		}
	}
}
