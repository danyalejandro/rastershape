import QtQuick 2.3

Grid {
	id: rootTiles
	columns: 2
	spacing: 0
	width: imgMapa1.width + imgMapa2.width
	height: imgMapa1.height + imgMapa3.height
	property string source
	property int cargados: 0 // Cuenta de imágenes ya cargadas
	property real esc: 1 // copia local almacenada de escala

	onCargadosChanged: {
		console.log("cargados: " + cargados)
		if (cargados == 4) {
			root.desbloquear()
			aplicarEscala(esc)
		}
	}

	ImageTile {
		id: imgMapa1
		onStatusChanged: {
			if (status == Image.Ready) {
				parent.cargados++
			}
		}
	}
	ImageTile {
		id: imgMapa2
		onStatusChanged: if (status == Image.Ready) parent.cargados++
	}
	ImageTile {
		id: imgMapa3
		onStatusChanged: if (status == Image.Ready) parent.cargados++
	}
	ImageTile {
		id: imgMapa4
		onStatusChanged: if (status == Image.Ready) parent.cargados++
	}

	// Libera el contenido de memoria
	function liberar() {
		imgMapa1.source = ""
		imgMapa2.source = ""
		imgMapa3.source = ""
		imgMapa4.source = ""
	}

	// vuelve invisible el contenido
	function ocultar() {
		imgMapa1.visible = false
		imgMapa2.visible = false
		imgMapa3.visible = false
		imgMapa4.visible = false
	}

	// Muestra los 4 tiles
	function mostrar() {
		imgMapa1.visible = true
		imgMapa2.visible = true
		imgMapa3.visible = true
		imgMapa4.visible = true
	}

	// Recarga todos los tiles
	// Parámetros opcionales: MBR del área a recargar (punto sup-izq, punto inf-der)
	function recargar(x1, y1, x2, y2) {
		var r = Math.random()
		var c1 = true, c2 = true, c3 = true, c4 = true

		console.log("tilesMapa.recargar()", x1, y1, x2, y2)
		root.bloquear()

		if (x1 != null) {
			// Se pasó una MBR: ver cuáles cuadrantes hay que recargar
			var w1 = imgMapa1.sourceSize.width, h1 = imgMapa1.sourceSize.height
			c1 = (x1 <= w1 && y1 <= h1)
			c2 = (x2 >= w1 && y1 <= h1)
			c3 = (x1 <= w1 && y2 >= h1)
			c4 = (x2 >= w1 && y2 >= h1)
		}

		var ruta = "image://imagen/" + source + "/"
		var pen = (c1 ? 1 : 0) + (c2 ? 1 : 0) + (c3 ? 1 : 0) + (c4 ? 1 : 0) // pendientes por cargar
		rootTiles.cargados = 4 - pen
		console.log(rootTiles.cargados)
		if (c1) { imgMapa1.source = ruta + miProxy.getCoordsUri(0) + r }
		if (c2) { imgMapa2.source = ruta + miProxy.getCoordsUri(1) + r }
		if (c3) { imgMapa3.source = ruta + miProxy.getCoordsUri(2) + r }
		if (c4) { imgMapa4.source = ruta + miProxy.getCoordsUri(3) + r }


		console.log("dimensiones tiles: ", rootTiles.width, rootTiles.height)
	}


	// Redimensiona todos los tiles en base a la escala
	function aplicarEscala(escala) {
		imgMapa1.width = imgMapa1.sourceSize.width * escala
		imgMapa1.height = imgMapa1.sourceSize.height * escala
		imgMapa2.width = imgMapa2.sourceSize.width * escala
		imgMapa2.height = imgMapa2.sourceSize.height * escala
		imgMapa3.width = imgMapa3.sourceSize.width * escala
		imgMapa3.height = imgMapa3.sourceSize.height * escala
		imgMapa4.width = imgMapa4.sourceSize.width * escala
		imgMapa4.height = imgMapa4.sourceSize.height * escala
		esc = escala
	}
}
