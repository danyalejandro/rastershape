// Ajusta el tamaño del miniMapa y su región según el tamaño de la imagen y ventana
function reajustar() {
	var imgAncho = tilesMapa.width
	var imgAlto = tilesMapa.height
	//console.log("(ajustar) dims imagen: ", imgAncho, imgAlto)

	if (flick.width >= imgAncho && flick.height >= imgAlto) {
		// no mostrar miniMapa si la imagen es más pequeña que la ventana
		miniMapaRegion.visible = false
		miniMapa.visible = false
	}
	else {
		miniMapaRegion.visible = true
		miniMapa.visible = true

		miniMapa.height = imgAlto * miniMapa.width / imgAncho;

		if (flick.width < imgAncho) {
			miniMapaRegion.width = (miniMapa.width * flick.width / imgAncho)
		} else {
			miniMapaRegion.width = miniMapa.width
		}

		if (flick.height < imgAlto) {
			miniMapaRegion.height = (miniMapa.height * flick.height / imgAlto)
		} else {
			miniMapaRegion.height = miniMapa.height
		}

		actPosX()
		actPosY()
	}
}

// Actualiza la posición x del minimapa
function actPosX() {
	if (miniMapa.visible) {
		miniMapaRegion.x = miniMapaRegion.width * flick.contentX / flick.width
	}
}
// Actualiza la posición y del minimapa
function actPosY() {
	if (miniMapa.visible) {
		miniMapaRegion.y = miniMapaRegion.height * flick.contentY / flick.height
	}
}
