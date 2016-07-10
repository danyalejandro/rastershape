// Devuelve la posición real del mouse en la imagen, independiente del zoom
function posReal(mouse, escala) {
	return Qt.point(mouse.x / escala, mouse.y / escala)
}

// Reacciona a cambios en el código de actividad
function onActividadChanged() {
	switch (actividad) {
		case 0: // Nada, permitir zoom & pan
			mapa.setMouseCursor(Qt.ArrowCursor)
			barraEstado.texto = ""
			mapa.ocultarCanvas()
			break
		case 1: // Ubicar las 4 marcas de referencia
			mapa.setMouseCursor(Qt.CrossCursor)
			barraEstado.texto = "Haga click para ubicar las 4 marcas de referencia e introduzca sus coordenadas planas en metros."
			break
		case 2: // Marcar trazos
			barraEstado.texto = "Haga click en las líneas para marcarlas."
			mapa.setMouseCursor(Qt.PointingHandCursor)
			mapa.ocultarCanvas()
			break
		case 3: // Lápiz
			mapa.mostrarCanvas(true)
			break
		case 4: // Borrador
			mapa.mostrarCanvas(false)
			break
		case 5: // recortar imagen
			barraEstado.texto = "Arrastre el área de recorte por los bordes. Para recortar, haga click derecho en el área de recorte."
			break
	}
}

// Procesar clicks en el MouseArea del mapa (no el mousearea del canvas de dibujo)
function mapaClick(mouse) {
	switch (root.actividad) {
		case 0: // Nada cargado
			break

		case 1: // Ubicar marcas para recorte
			LogEtaCorte.agregarMarca(mouse)
			break

		case 2: // Marcar contornos
			var posR = posReal(mouse, mapa.escala)
			console.log("pos real: ", posR.x, posR.y)

			if (Glo.hayTrazoRojo) {
				miProxy.guardarTrazo()
				Glo.hayTrazoVerde = true
			}

			miProxy.marcarTrazo(
					posR.x,	posR.y,
					Number(pin_radio.currentText),
					Number(pin_vecinos.currentText),
					Number(pin_cambioAng.value) * 0.0174532925)

			Glo.hayTrazoRojo = true
			pin_btBorrarCapa.enabled = true

			mapa.actualizTrazos()
			break
	}
}

// Carga una imagen de la ruta
function cargarImagen(ruta) {
	root.actividad = 0
	mapa.ocultarCanvas()
	marca1.reset()
	marca2.reset()
	marca3.reset()
	marca4.reset()
	Glo.hayTrazoRojo = false
	Glo.hayTrazoVerde = false
	// ocultamos para liberar memoria (@todo: refactorizar)
	mapa.ocultarTrazos()
	mapa.mostrarTrazos()

	miProxy.cargarImagen(ruta)

	mapa.actualizImagen()

	barraTabs.habilitarTab(1)
	barraTabs.habilitarTab(2)
	barraTabs.habilitarTab(3)
	barraTabs.habilitarTab(4)
	barraTabs.habilitarTab(5)
}

// Aplica limpieza de pixeles y trazos aislados
// También aplica limpieza de bordes
function limpiarAislados() {
	miProxy.limpiarAislados()
	miProxy.limpiarBordes()
	mapa.actualizImagen()
}

// aplica adelgazamiento
function adelgazar(desenfocar) {
	console.log("adelgazar() rojos:" + Glo.hayTrazoRojo + ", verdes: " + Glo.hayTrazoVerde)
	if (Glo.hayTrazoRojo) {
		miProxy.guardarTrazo()
		Glo.hayTrazoVerde = true
		Glo.hayTrazoRojo = false
	}

	if (Glo.hayTrazoVerde) {
		// Aún no se aislan los trazos
		LogEtaPincel.aislarTrazos()
	}

	miProxy.adelgazar(desenfocar)

	mapa.origenTrazos("pincel")
	Glo.hayTrazoVerde = true
	mapa.actualizTrazos()
	mapa.mostrarTrazos()
	mapa.actualizImagen()
}

// Aplica reconstrucción
function reconstruir() {
	miProxy.reconstruir(Number(rec_mascara.text))
	mapa.actualizImagen()
}

// Realiza la vectorización y guarda el vector resultante en la ubicación solicitada
function guardarVector(fileUrl) {
	miProxy.vectorizar(Number(vect_reducPuntos.value), Number(vect_suavizado.value), fileUrl)
}

// Guarda la imagen actual en la ruta
function guardarImagen(fileUrl) {
	miProxy.guardarImagen(fileUrl)
}

// Devuelve la posición real del mouse en la imagen, independiente del zoom
function posReal(mouse, escala) {
	return Qt.point(mouse.x / escala, mouse.y / escala)
}


