import QtQuick 2.3
import QtQuick.Layouts 1.1
import QtQuick.Controls 1.2
import QtQuick.Controls.Styles 1.2

// Barra de herramientas tabulada con estilo gráfico Office
Item {
	id: miBarraTab
	width: parent.width
	height: 24 + 43

	property var pestanas
	property var estados
	default property alias contents: conTabs.children

	function mostrarTab(numTab) {
		var numTabs = conTabs.children.length

		for (var i = 0 ; i < numTabs ; ++i) {
			if (numTab !== i) {
				if (conBotonesTab.children[i].estado === 1) {
					conBotonesTab.children[i].desactivar()
				}
				conTabs.children[i].visible = false
			}
		}

		conBotonesTab.children[numTab].estado = 1
		conTabs.children[numTab].visible = true
	}

	function habilitarTab(numTab) {
		conBotonesTab.children[numTab].estado = 0;
	}
	function deshabilitarTab(numTab) {
		conBotonesTab.children[numTab].estado = -1;
	}

	Component.onCompleted: {
		var numTabs = conTabs.children.length
		for (var i = 0 ; i < numTabs ; ++i) {
			conTabs.children[i].visible = false
		}

		mostrarTab(0)
	}

	Rectangle {
		id: tabBotones
		width: parent.width
		height: 24

		gradient: Gradient {
			GradientStop { position: 0.0; color: "#E8ECF0" }
			GradientStop { position: 1.0; color: "#FAFCFC" }
		}

		Rectangle {
			anchors.bottom: parent.bottom
			width: parent.width
			height: 1
			color: "#B6BABF"
		}

		Row {
			id: conBotonesTab

			Repeater {
				model: miBarraTab.pestanas
				CanvasTab {
					text: modelData
					indice: index
					estado: miBarraTab.estados[index]
					onClicked: miBarraTab.mostrarTab(index)
				}
			}
		}
	}

	Rectangle {
		anchors.top: tabBotones.bottom
		height: 43
		width: parent.width

		gradient: Gradient {
			GradientStop { position: 0.0; color: "#FFFFFF" }
			GradientStop { position: 0.7; color: "#FFFFFF" }
			GradientStop { position: 1.0; color: "#EDF1F4" }
		}
		// Línea borde bottom
		Rectangle {
			anchors.bottom: parent.bottom
			width: parent.width
			height: 1
			color: "#B9C6D2"
		}

		Item {
			id: conTabs
			x: 5
			width: parent.width
			height: parent.height
		}
	}
}
