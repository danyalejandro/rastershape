import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.1

ApplicationWindow {
	id: root
	width: 452
	height: 300
	minimumHeight: height
	minimumWidth: width
	maximumHeight: height
	maximumWidth: width
	modality: Qt.ApplicationModal
	flags: Qt.Dialog
	title: "Coordenadas Planas"

	GroupBox {
		width: parent.width - 30
		height: 60
		x: 15
		y: 15
		title: ""
		id: gr1

		Text {
			anchors.fill: parent
			anchors.margins: 10
			wrapMode: Text.WordWrap
			text: "Ingrese las coordenadas planas (en metros) de los 4 puntos que referencian el mapa en sus esquinas."
		}
	}

	GridLayout {
		anchors.top: gr1.bottom
		anchors.topMargin: 15
		x: 15
		columns: 4
		columnSpacing: 24
		rowSpacing: 10

		Text { text: ""; font.bold: true; }
		Text { text: "Ubicación"; font.bold: true; }
		Text { text: "Norte"; font.bold: true; }
		Text { text: "Este"; font.bold: true; }

		Text { text: "A." }
		Text { text: "Arriba - Izquierda" }
		TextField { id: aEste }
		TextField { id: aNorte }

		Text { text: "B." }
		Text { text: "Arriba - Derecha" }
		TextField { id: bEste }
		TextField { id: bNorte }

		Text { text: "C." }
		Text { text: "Abajo - Izquierda" }
		TextField { id: cEste }
		TextField { id: cNorte }

		Text { text: "D." }
		Text { text: "Abajo - Derecha" }
		TextField { id: dEste }
		TextField { id: dNorte }
	}


	Button {
		id: btCancelar
		anchors.bottom: parent.bottom
		anchors.right: parent.right
		anchors.margins: 15
		text: "Cancelar"
		onClicked: root.close()
	}

	Button {
		id: btAceptar
		anchors.bottom: parent.bottom
		anchors.right: btCancelar.left
		anchors.margins: 15
		text: "Aceptar"
		onClicked: {
			miProxy.setCoordsMarcas(Number(aEste.text), Number(aNorte.text), Number(bEste.text), Number(bNorte.text), Number(cEste.text), Number(cNorte.text), Number(dEste.text), Number(dNorte.text))
			root.close()
		}
	}
}
