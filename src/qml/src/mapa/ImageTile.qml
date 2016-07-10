import QtQuick 2.3

Image {
	cache: false
	smooth: false
	clip: false
	asynchronous: true // esperar para hacer cálculos de ancho y alto
	transformOrigin: Item.TopLeft
}
