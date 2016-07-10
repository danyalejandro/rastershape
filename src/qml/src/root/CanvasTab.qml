import QtQuick 2.3

Canvas {
	id: canvas
	width: Math.floor(titulo.width) + 32
	height: 24
	antialiasing: false
	smooth: false

	property int indice: 0
	property int estado: 0 // 0: inactiva, 1: activa, -1: deshabilitada
	property alias text: titulo.text
	property color textColor: (estado == 1) ? "#1B6CBC" : ((estado == -1) ? "#999999" : "#222222")

	onEstadoChanged: redibujar()
	onTextChanged: redibujar()

	signal clicked(var mouse)

	function redibujar() {
		switch (estado) {
		case -1: deshabilitar(); break;
		case 0: habilitar(); break;
		case 1: habilitar(); break;
		}

		requestPaint()
	}

	// Invocar para desactivar esta pestaña
	function desactivar() {
		if (estado !== 0) {
			estado = 0
			textColor = "#222222"
			redibujar()
		}
	}

	// Evitar que la pestaña responda a eventos del mosuse
	function deshabilitar() {
		textColor = "#999999"
	}

	function habilitar() {
		textColor = "#222222"
	}

	Component.onCompleted: {
		loadImage("qrc:/img/tab-borde-izq.png")
		loadImage("qrc:/img/tab-borde-der.png")
	}

	onPaint: {
		var ctx = canvas.getContext("2d")
		ctx.clearRect(0,0,width,height)

		if (estado === 0) {
			// Normal
			ctx.beginPath()
			ctx.moveTo(0, canvas.height - 1)
			ctx.lineTo(canvas.width, canvas.height - 1)
			ctx.strokeStyle = '#B6BABF'
			ctx.stroke()
		}
		else if (estado === 1) {
			// Activo
			ctx.beginPath()
			ctx.moveTo(3, 0)
			ctx.lineTo(canvas.width - 3, 0)
			ctx.strokeStyle = '#B6BABF'
			ctx.stroke()

			ctx.rect(3, 1, canvas.width - 6, canvas.height);
			ctx.fillStyle = '#FFFFFF';
			ctx.fill();

			ctx.drawImage("qrc:/img/tab-borde-izq.png", 0, 0)
			ctx.drawImage("qrc:/img/tab-borde-der.png", canvas.width - 3, 0)
		}
	}

	Text {
		id: titulo
		anchors.centerIn: parent
		text: "Tabname"
		color: textColor
	}

	MouseArea {
		id: mouseArea
		anchors.fill: parent
		hoverEnabled: true
		onClicked: {
			if (parent.estado !== -1) parent.clicked(mouse)
		}
		onContainsMouseChanged: {
			if (canvas.estado === 0) {
				canvas.textColor = (containsMouse) ? "#1B6CBC" : "#222222"
			}
		}
	}
}
