// Devuelve la posición real del mouse en la imagen, independiente del zoom
function posReal(mouse, escala) {
	return Qt.point(mouse.x / escala, mouse.y / escala)
}

function actualizImagen() {
	tilesMapa.recargar()
	miniMapa.reajustar()
	gc()
}

// Establece un tipo de cursor de mouse
function setMouseCursor(cod) {
	areaMouse.cursorShape = cod
}

function actualizTrazos() {
	if (Glo.hayTrazoRojo || Glo.hayTrazoVerde) {
		tilesTrazos.recargar()
		areaZoom(2, flick.contentX + flick.width / 2, flick.contentY + flick.height / 2)
	}
}

function ocultarTrazos() {
	tilesTrazos.ocultar()
}

function liberarTrazos() {
	tilesTrazos.liberar()
}

function mostrarTrazos() {
	tilesTrazos.mostrar()
}

// Retorna el contenido a la posición inicial
function posInit() {
	flick.contentX = 0
	flick.contentY = 0
	flick.returnToBounds()
}

// Retorna a zoom 100%
function zoomInit() {
	areaZoom(0, flick.contentX + flick.width / 2, flick.contentY + flick.height / 2)
}

// Aumenta el nivel de zoom en 1 nivel
function zoomIn() {
	areaZoom(1, flick.contentX + flick.width / 2, flick.contentY + flick.height / 2)
}

// reduce el nivel de zoom en 1 nivel
function zoomOut() {
	areaZoom(-1, flick.contentX + flick.width / 2, flick.contentY + flick.height / 2)
}

// Resetea el pan y el zoom
function zoomPanReset() {
	posInit()
	zoomInit()
}

// Calcula el factor de escala a partir del número de zoom actual
function are_calcEscala(numZoom) {
	if (numZoom === 0) return 1
	return Math.pow(2, numZoom)
}


// Aplica un tipo de zoom según su código
// -1: zoomOut, 1: zoomIn, 0: tamaño real, 2: re-aplicar zoom
function areaZoom(tipoZoom, centroX, centroY) {
	var valido = false
	var escalaAnt = mapa.escala
	var conXant = flick.contentX
	var conYant = flick.contentY

	// interpretamos el tipo de zoom
	switch (tipoZoom) {
	case -1: // Zoom Out
		if (mapa.numEscala > -4) {
			mapa.numEscala--
			valido = true
		}
		break
	case 0: // 100%
		mapa.numEscala = 0
		valido = true
		break
	case 1: // Zoom In
		if (mapa.numEscala < 5) {
			mapa.numEscala++
			valido = true
		}
		break
	case 2: // Mismo zoom (re-aplicar)
		valido = true
		break
	}

	if (valido) {
		mapa.escala = are_calcEscala(mapa.numEscala)
		var fh = flick.height
		var fw = flick.width
		tilesMapa.aplicarEscala(mapa.escala)
		if (Glo.hayTrazoVerde || Glo.hayTrazoRojo) tilesTrazos.aplicarEscala(mapa.escala)

		// Reubicamos para mantener el centro del zoom
		var rel = mapa.escala / escalaAnt
		var difX, difY
		if (tipoZoom === 1) {
			difX = centroX * (rel - 1)
			difY = centroY * (rel - 1)
		} else {
			difX = -centroX * (1 - rel)
			difY = -centroY * (1 - rel)
		}

		var cx = conXant + Math.floor(difX)
		var cy = conYant + Math.floor(difY)
		var ih = tilesMapa.height
		var iw = tilesMapa.width

		// Reubicamos si estamos en una posición inválida
		if (cx < 0 || iw < fw) { cx = 0 }
		if (cy < 0 || ih < fh) { cy = 0 }
		if (ih > fh && ((ih - cy) < fh)) { cy = ih - fh }
		if (iw > fw && ((iw - cx) < fw)) { cx = iw - fw }
		//console.log("cpos: (" + cx + "," + cy + "); i: (" + iw + "," + ih + "); f: (" + fw + "," + fh + ")")

		flick.contentX = cx
		flick.contentY = cy
		miniMapa.reajustar()
		canvasMapa.actualizarEscala()

		// actualizamos el slider
		if (zoom_slider.value !== mapa.numEscala) zoom_slider.value = mapa.numEscala

		// desplazamos los elementos contenidos
		var hijos = contenido.children.length
		for (var i = 0 ; i < hijos ; ++i) {
			contenido.children[i].actPos(mapa.escala, escalaAnt)
		}
	}
}
