import QtQuick 2.3
Item {
	id: resRect
	property int anchoBorde: 1
	property color colBorde: "#2C7EB6"
	property point posIni
	property real escala // almacena la escala del mapa

	signal clickDer // Al hacerle click derecho

	// establece la posición y tamaño
	function setRect(x, y, ancho, alto) {
		rIzq.x = x
		rArr.y = y
		rDer.x = ancho
		rAba.y = alto
	}

	// Obtiene el rectángulo contenedor real
	function getRect() {
		return Qt.rect(Math.round(rIzq.x / escala),
					   Math.round(rArr.y / escala),
					   Math.round((rDer.x - rIzq.x) / escala),
					   Math.round((rAba.y - rArr.y) / escala))
	}

	// Actualiza la posición de la marca según una escala
	function actPos(escalaSig, escalaAnt) {
		if (visible) {
			escala = escalaSig
			var cambioEsc = (escalaSig / escalaAnt)
			var anchoIni = rArr.width
			var altoIni = rIzq.height
			var xIni = rIzq.x
			var yIni = rArr.y
			rIzq.x = xIni * cambioEsc
			rArr.y = yIni * cambioEsc
			rDer.x = rIzq.x + anchoIni * cambioEsc
			rAba.y = rArr.y + altoIni * cambioEsc
		}
	}

	Rectangle {

		width: rArr.width
		height: rIzq.height
		x: rArr.x
		y: rArr.y
		color: "#90C1E3"
		opacity: 0.5

		Component.onCompleted: {
			rArr.y = 20
			rAba.y = 120
			rDer.x = 120
			rIzq.x = 20
			posIni = Qt.point(rIzq.x, rArr.y)
		}

		MouseArea {
			anchors.fill: parent
			acceptedButtons: Qt.LeftButton | Qt.RightButton
			onClicked: {
				console.log(getRect())
				if (mouse.button === Qt.RightButton) {
					clickDer()
				}
			}
		}
	}



	Rectangle {
		id: rArr
		width: rDer.x - rIzq.x
		x: rIzq.x
		height: resRect.anchoBorde
		color: resRect.colBorde

		Rectangle {
			width: 7; height: 7
			anchors.verticalCenter: parent.verticalCenter
			anchors.horizontalCenter: parent.horizontalCenter
			border.color: "#000000"
		}

		MouseArea {
			y: -5
			width: parent.width
			height: 10
			drag.target: parent
			cursorShape: Qt.SizeVerCursor
			drag.axis: Drag.YAxis
			drag.maximumY: rAba.y - 40
			drag.minimumY: 0
		}
	}

	Rectangle {
		id: rAba
		width: rArr.width
		height: resRect.anchoBorde
		x: rArr.x
		color: resRect.colBorde

		Rectangle {
			width: 7; height: 7
			anchors.verticalCenter: parent.verticalCenter
			anchors.horizontalCenter: parent.horizontalCenter
			border.color: "#000000"
		}

		MouseArea {
			y: -5
			width: parent.width
			height: 10
			drag.target: parent
			cursorShape: Qt.SizeVerCursor
			drag.axis: Drag.YAxis
			drag.minimumY: rArr.y + 40
		}
	}

	Rectangle {
		id: rIzq
		width: resRect.anchoBorde
		height: rAba.y - rArr.y
		y: rArr.y
		color: resRect.colBorde

		Rectangle {
			width: 7; height: 7
			anchors.verticalCenter: parent.verticalCenter
			anchors.horizontalCenter: parent.horizontalCenter
			border.color: "#000000"
		}
		Rectangle {
			width: 7; height: 7
			y: -3
			anchors.horizontalCenter: parent.horizontalCenter
			border.color: "#000000"
		}
		Rectangle {
			width: 7; height: 7
			y: parent.height - 3
			anchors.horizontalCenter: parent.horizontalCenter
			border.color: "#000000"
		}

		MouseArea {
			x: -5
			height: parent.height
			width: 10
			drag.target: parent
			cursorShape: Qt.SizeHorCursor
			drag.axis: Drag.XAxis
			drag.maximumX: rDer.x - 40
		}
	}

	Rectangle {
		id: rDer
		width: resRect.anchoBorde
		height: rIzq.height
		y: rIzq.y
		color: resRect.colBorde

		Rectangle {
			width: 7; height: 7
			anchors.verticalCenter: parent.verticalCenter
			anchors.horizontalCenter: parent.horizontalCenter
			border.color: "#000000"
		}
		Rectangle {
			width: 7; height: 7
			y: -3
			anchors.horizontalCenter: parent.horizontalCenter
			border.color: "#000000"
		}
		Rectangle {
			width: 7; height: 7
			y: parent.height - 3
			anchors.horizontalCenter: parent.horizontalCenter
			border.color: "#000000"
		}

		MouseArea {
			x: -5
			height: parent.height
			width: 10
			drag.target: parent
			cursorShape: Qt.SizeHorCursor
			drag.axis: Drag.XAxis
			drag.minimumX: rIzq.x + 40
		}
	}
}
