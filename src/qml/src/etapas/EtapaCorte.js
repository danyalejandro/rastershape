// Funcionalidad relacionada con la actividad de ubicar marcas para recorte

// Reinicia todas las marcas
function resetMarcas() {
	marca1.reset()
	marca2.reset()
	marca3.reset()
	marca4.reset()
}

function ajustarMarcas() {
	// Reposicionamos las marcas
	marca1.actPos(mapa.escala)
	marca2.actPos(mapa.escala)
	marca3.actPos(mapa.escala)
	marca4.actPos(mapa.escala)
}

// Agrega una nueva marca en la posición del mouse
function agregarMarca(mouse) {
	if (!marca1.visible) {
		marca1.ubicar(mouse.x, mouse.y, mapa.escala)
	}
	else if (!marca2.visible) {
		marca2.ubicar(mouse.x, mouse.y, mapa.escala)
	}
	else if (!marca3.visible) {
		marca3.ubicar(mouse.x, mouse.y, mapa.escala)
	}
	else if (!marca4.visible) {
		marca4.ubicar(mouse.x, mouse.y, mapa.escala)
		btAgrMarca.checked = false
		btAgrMarca.enabled = true
		root.actividad = 0
	}
}

// Recorta la imagen dentro de la selección
function recortarImagen() {
	var marcas = [marca1, marca2, marca3, marca4]

	var r = rectRecorte.getRect()
	miProxy.recortar(r.x, r.y, r.width, r.height)
	mapa.actualizImagen()
	mapa.zoomPanReset()
	rectRecorte.visible = false

	// Actualizamos controles
	var rx, ry
	for (var i = 0 ; i < 4 ; i++) {
		marcas[i].visible = false
	}
}
