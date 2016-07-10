// Lógica de CanvasMap

function limpiar() {
	ctx.restore()
	ctx.clearRect(0, 0, width, height)
	actualizarEscala()
}

// Actualiza el canvas a la escala de la imagen
function actualizarEscala() {
	ctx.restore()
	ctx.save()
	offsetX = -flick.contentX
	offsetY = -flick.contentY
	ctx.translate(offsetX, offsetY)
	ctx.scale(escala, escala)
	requestPaint()
}

// Obtiene la posX del mouse, en escala 1:1 (sin importar zoom o pan)
function coordRealX(v) {
	return Math.floor((v + flick.contentX) / mapa.escala)
}
function coordRealY(v) {
	return Math.floor((v + flick.contentY) / mapa.escala)
}

// Dibuja un "pixel" en el canvas y en la imagen binaria
// recibe las coords llanas del click en el área
function dibujarPix(mx, my) {
	var rx = coordRealX(mx)
	var ry = coordRealY(my)
	//console.log("dibujarPix() c: ", flick.contentX, flick.contentY, "; mouse: ", mx, my, "; real: (", rx , ",", ry, ")")

	var tam = rootCanvas.tamanoLapiz
	if (tam == 0) {
		// dibujamos 1 pixel
		ctx.fillRect(rx, ry, 1, 1)
	}
	else {
		// dibujamos una cruz de 3x3
		ctx.fillRect(rx - 1, ry, 3, 1)
		ctx.fillRect(rx, ry - 1, 1, 3)
	}

	miProxy.pintar(rx, ry, rootCanvas.pintar, tam)
	requestPaint()
}


// **** MOUSE AREA

// mx, my: coordenadas de click (llanas, desde la esquina sup izq del área)
function evMousePress(mx, my) {
	mArea.xAnt = mx
	mArea.yAnt = my

	mArea.xMin = mx
	mArea.xMax = mx
	mArea.yMin = my
	mArea.yMax = my

	rootCanvas.setColor()

	dibujarPix(mx, my)
}

function evMouseUp(mouse) {
	var x1 = coordRealX(mArea.xMin)
	var y1 = coordRealY(mArea.yMin)
	var x2 = coordRealX(mArea.xMax)
	var y2 = coordRealY(mArea.yMax)
	//console.log("evMouseUp() rect: ",x1,y1,x2,y2)

	if (Glo.hayTrazoRojo) {
		miProxy.reintentarRecorrido()
		tilesTrazos.recargar()
	}

	tilesMapa.recargar()
	limpiar(flick.contentX, flick.contentY) // referencias?
}

function evMouseMove(mouse, pressed) {
	sombraMouse.actPos(mouse.x, mouse.y, offsetX, offsetY)
	if (!sombraMouse.visible) sombraMouse.visible = true

	if (pressed) {
		var dx, dy, fx, fy, xAct, yAct, xDist, yDist;
		var xSig = mouse.x
		var ySig = mouse.y

		xAct = mArea.xAnt
		yAct = mArea.yAnt
		dx = xSig - xAct // pixeles
		dy = ySig - yAct
		fx = (dx > 0) ? 1 : -1 // sentido del despl
		fy = (dy > 0) ? 1 : -1
		xDist = Math.abs(dx) // Distancia al punto final
		yDist = Math.abs(dy)

		while (xDist > 1 || yDist > 1) {
			if (xDist > yDist) {
				xAct += fx
			}
			else if (yDist > xDist) {
				yAct += fy
			}
			else {
				xAct += fx
				yAct += fy
			}

			dibujarPix(xAct, yAct)

			xDist = Math.abs(xAct - xSig)
			yDist = Math.abs(yAct - ySig)
		}

		dibujarPix(xSig, ySig, rootCanvas.tamanoLapiz) // dibujar el punto final (el inicial ya está)

		mArea.xAnt = xSig
		mArea.yAnt = ySig

		if (xSig < mArea.xMin) mArea.xMin = xSig
		else if (xSig > mArea.xMax) mArea.xMax = xSig
		if (ySig < mArea.yMin) mArea.yMin = ySig
		else if (ySig > mArea.yMax) mArea.yMax = ySig
	}
}
