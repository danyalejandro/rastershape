import QtQuick 2.3
import "MiniMapa.js" as Log

// Minimapa
Rectangle {
	id: miniMapa
	opacity: 0.85
	color: "#86ADFD"
	border.color: "#3B62B1"
	border.width: 1
	smooth: false

	function reajustar() { Log.reajustar() }
	function actPosX() { Log.actPosX() }
	function actPosY() { Log.actPosY() }

	// Región visible
	Rectangle {
		id: miniMapaRegion
		width: 50
		height: 45
		color: "#FFCCAA"
		border.color: "#DF5732"
		visible: false
		smooth: false
	}
}
