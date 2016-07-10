import QtQuick 2.3
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.1
import QtQuick.Dialogs 1.2

import "root" as Root
import "mapa" as Mapa
import "etapas" as Etapas
import "frm" as Frm

import "etapas/EtapaCorte.js" as LogEtaCorte
import "etapas/EtapaPincel.js" as LogEtaPincel
import "main.js" as LogMain
import "global.js" as Glo

ApplicationWindow {
	id: root
	visible: true
	width: 1024
	height: 768
	visibility: "Maximized"
	title: "RasterShape"
	color: "#E9EDF3"

	property int actividad: 0

	function bloquear() {
		barraOcupado.visible = true
	}
	function desbloquear() {
		barraOcupado.visible = false
	}

	function mapaClick(mouse) { LogMain.mapaClick(mouse) }


	onWidthChanged: mapa.ajustarMinimapa()
	onHeightChanged: mapa.ajustarMinimapa()
	onActividadChanged: LogMain.onActividadChanged()

	// Diálogo para guardar coordenadas de recorte
	Frm.CoordsCorte {
		id: frmCoordsCorte
		visible: false
	}

	// Diálogo para abrir imagen
	FileDialog {
		id: dialAbrir
		title: "Seleccione una imagen"
		nameFilters: [ "Archivos de imagen (*.bmp *tif *.png)" ]
		onAccepted: {
			LogMain.cargarImagen(fileUrl)
			btGuardarImagen.enabled = true
		}
	}
	FileDialog {
		id: dialGuardarVect
		selectExisting: false
		title: "Guardar vector como..."
		nameFilters: [ "Archivo SHAPE (*.shp)" ]
		onAccepted: LogMain.guardarVector(fileUrl)
	}
	FileDialog {
		id: dialGuardarImg
		selectExisting: false
		title: "Guardar imagen como..."
		nameFilters: [ "Imagen PNG (*.png)" ]
		onAccepted: LogMain.guardarImagen(fileUrl)
	}
	MessageDialog {
		id: dialConfRecortar
		title: "¿Recortar imagen?"
		text: "Recortar la imagen entre las marcas del área de mapa, reduce la cantidad de memoria necesaria para trabajar con imágenes grandes. El archivo original de la imagen no será modificado.\n\n¿Desea recortar la imagen?"
		icon: StandardIcon.Question
		standardButtons: StandardButton.Ok | StandardButton.Cancel
		onAccepted: LogEtaCorte.recortarImagen()
	}

	// Barra tabulada de herramientas
	Root.OfficeTabBar {
		id: barraTabs
		y: 3
		pestanas: ["Inicio", "Área de trabajo", "Marcado de líneas", "Adelgazamiento", "Reconstrucción", "Vectorización"]
		estados: [1,-1,-1,-1,-1,-1]

		// Tab Inicio
		Root.OfficeTabRow {
			ToolButton {
				iconSource: "qrc:/img/folder.png"
				tooltip: "Abrir imagen..."
				anchors.verticalCenter: parent.verticalCenter
				onClicked: dialAbrir.open()
			}
			Root.ToolButtonSt {
				id: btGuardarImagen
				imgEnabled: "qrc:/img/disk.png"
				imgDisabled: "qrc:/img/disk2.png"
				tooltip: "Guardar imagen como..."
				enabled: false
				onClickSt: dialGuardarImg.open()
			}


		}

		// Tab Área de trabajo
		Root.OfficeTabRow {
			RowLayout {
				height: parent.height

				Root.ButtonSt {
					id: btAgrMarca
					imgEnabled: "qrc:/img/marca.png"
					imgDisabled: "qrc:/img/marca2.png"
					tooltip: "Ubicar marcas de referencia"
					text: "Marcas de referencia..."
					width: 144
					checkable: true

					onClickSt: {
						if (checked) {
							LogEtaCorte.resetMarcas()
							root.actividad = 1
						}
						else {
							if (!marca4.visible) LogEtaCorte.resetMarcas()
							root.actividad = 0
						}
					}
				}
			}

			/*Root.ButtonSt {
				id: btFrmCoords
				imgEnabled: "qrc:/img/marker.png"
				imgDisabled: "qrc:/img/marker2.png"
				text: "Coordenadas planas..."

				onClickSt: {
					frmCoordsCorte.visible = true
				}
			}*/

			Root.ButtonSt {
				id: btRecortar
				imgEnabled: "qrc:/img/crop.png"
				imgDisabled: "qrc:/img/crop2.png"
				tooltip: "Recortar imagen..."
				text: "Recortar imagen"
				checkable: true
				enabled: true

				onClickSt: {
					if (checked) {
						rectRecorte.escala = mapa.escala
						rectRecorte.setRect(0, 0, mapa.getAnchoMapa(), mapa.getAltoMapa())
						rectRecorte.visible = true
						root.actividad = 5
					} else {
						rectRecorte.visible = false
						root.actividad = 0
					}
				}
			}

			Root.ButtonSt {
				id: btLimpiar
				anchors.verticalCenter: parent.verticalCenter
				imgEnabled: "qrc:/img/limpiar-aislados.png"
				text: "Limpiar ruido"
				tooltip: "Limpiar ruido"
				onClickSt: LogMain.limpiarAislados()
			}
		}

		// Tab Marcado de líneas
		Root.OfficeTabRow {
			spacing: 24

			RowLayout {
				height: parent.height
				ToolButton {
					id: pin_btMarcar
					iconSource: "qrc:/img/select-line.png"
					tooltip: "Marcar trazo"
					checkable: true
					anchors.verticalCenter: parent.verticalCenter
					onClicked: {
						if (checked) {
							pin_btBorrador.checked = false
							pin_btLapiz.checked = false
							root.actividad = 2
						} else {
							root.actividad = 0
						}
					}
				}

				/*
				Root.ToolButtonSt {
					id: pin_btGuardar
					imgEnabled: "qrc:/img/guardartrazo.png"
					imgDisabled: "qrc:/img/guardartrazo2.png"
					enabled: false
					tooltip: "Guardar Trazo"
					onClickSt: {
						pin_btGuardar.enabled = false
						pin_btBorrarCapa.enabled = false
						btAislarTrazos.enabled = true
						pin_btLapiz.checked = false
						pin_btBorrador.checked = false
						miProxy.guardarTrazo()
						Glo.hayTrazoRojo = false
						Glo.hayTrazoVerde = true
						mapa.actualizTrazos()
						pin_btMarcar.checked = false
						root.actividad = 0
					}
				}*/

				Root.ToolButtonSt {
					id: pin_btBorrarCapa
					imgEnabled: "qrc:/img/borrartrazo.png"
					imgDisabled: "qrc:/img/borrartrazo2.png"
					tooltip: "Borrar trazo"
					enabled: false
					onClickSt: {
						pin_btBorrarCapa.enabled = false
						pin_btLapiz.checked = false
						pin_btBorrador.checked = false
						miProxy.borrarTrazos()
						mapa.actualizTrazos()
						Glo.hayTrazoRojo = false
						pin_btMarcar.checked = false
						root.actividad = 0
					}
				}
				Root.TabSeparator {
					anchors.leftMargin: 10
				}
				ToolButton {
					id: pin_btLapiz
					iconSource: "qrc:/img/pencil.png"
					tooltip: "Lapiz"
					checkable: true
					anchors.verticalCenter: parent.verticalCenter
					onClicked: {
						if (checked) {
							pin_btMarcar.checked = false
							pin_btBorrador.checked = false
							root.actividad = 3
						} else {
							root.actividad = 0
						}
					}
				}
				ToolButton {
					id: pin_btBorrador
					iconSource: "qrc:/img/draw_eraser.png"
					tooltip: "Borrador"
					checkable: true
					anchors.verticalCenter: parent.verticalCenter
					onClicked: {
						if (checked) {
							pin_btMarcar.checked = false
							pin_btLapiz.checked = false
							root.actividad = 4
						} else {
							root.actividad = 0
						}
					}
				}
				ToolButton {
					id: pin_btGrosor
					iconSource: "qrc:/img/grosor.png"
					tooltip: "Grosor de dibujo"
					anchors.verticalCenter: parent.verticalCenter
					menu: Menu {
						MenuItem {
							text: "Delgado"
							checkable: true
							exclusiveGroup: pin_grupoGrosor
							onTriggered: { mapa.tamanoLapiz = 0 }
						}
						MenuItem {
							text: "Ancho"
							checkable: true
							checked: true
							exclusiveGroup: pin_grupoGrosor
							onTriggered: { mapa.tamanoLapiz = 1 }
						}
					}
				}
				ExclusiveGroup { id: pin_grupoGrosor }

				Root.TabSeparator {
					anchors.leftMargin: 10
				}
			}

			Root.TabFormItem {
				text: "Radio:"
				ComboBox {
					id: pin_radio
					implicitWidth: 52
					model: [ "1.5", "2", "2.5", "3", "4", "5" ]
					currentIndex: 2
				}
			}

			Root.TabFormItem {
				text: "Vecinos:"
				ComboBox {
					id: pin_vecinos
					implicitWidth: 40
					model: [ "3", "5", "7", "8" ]
					currentIndex: 1
				}
			}

			Root.TabFormItem {
				text: "Cambio angular:"
				Slider {
					id: pin_cambioAng
					implicitWidth: 100
					stepSize: 10
					minimumValue: 40
					maximumValue: 180
					value: 90
				}
				Text {
					width: 30
					id: pin_camAngTxt
					text: pin_cambioAng.value + "°"
				}
			}


			RowLayout {
				height: parent.height
				spacing: 22

				Root.TabSeparator {	}

				Root.ButtonSt {
					id: btAislarTrazos
					imgEnabled: "qrc:/img/lineamarca.png"
					imgDisabled: "qrc:/img/lineamarca2.png"
					text: "Aislar marcas"
					enabled: true

					onClickSt: {
						if (Glo.hayTrazoRojo) {
							miProxy.guardarTrazo()
							Glo.hayTrazoRojo = false
							Glo.hayTrazoVerde = true
						}

						if (Glo.hayTrazoVerde) {
							btAislarTrazos.enabled = false
							pin_btBorrarCapa.enabled = false
							pin_btMarcar.checked = false
							LogEtaPincel.aislarTrazos()
							Glo.hayTrazoVerde = false
						}
					}
				}
			}
		}

		// Tab Adelgazamiento
		Root.OfficeTabRow {
			Rectangle {
				height: parent.height
				width: 124
				anchors.leftMargin: 14
				color: "transparent"

				CheckBox {
					id: ade_chkDesenfoque
					text: "Desenfocar trazos"
					checked: true
					anchors.verticalCenter: parent.verticalCenter
					x: 20
				}
			}

			Root.TabSeparator {	}

			Button {
				id: ade_btAdelgazar
				text: "Adelgazar"
				implicitWidth: 86
				implicitHeight: 28
				iconSource: "qrc:/img/adelgazar.png"
				anchors.verticalCenter: parent.verticalCenter

				onClicked: LogMain.adelgazar(ade_chkDesenfoque.checked)
			}
		}

		// Tab Reconstrucción
		Root.OfficeTabRow {
			x: 15
			Root.TabFormItem {
				text: "Tamaño de la mascara"
				TextField {
					id: rec_mascara
					implicitWidth: 35
					height: 20
					anchors.verticalCenter: parent.verticalCenter
					text: "40"
				}
			}

			Root.TabSeparator {	}

			Button {
				id: vec_btReconstruir
				text: "Reconstruir trazos"
				implicitHeight: 28
				implicitWidth: 126
				iconSource: "qrc:/img/recons.png"
				anchors.verticalCenter: parent.verticalCenter

				onClicked: LogMain.reconstruir()
			}
		}

		// Tab Vectorización
		Root.OfficeTabRow {
			x: 15
			Root.TabFormItem {
				text: "Reducción de puntos:"
				Rectangle {
					width: 160
					height: parent.height - 12
					anchors.leftMargin: 3
					Slider {
						id: vect_reducPuntos
						anchors.bottom: parent.bottom
						implicitWidth: parent.width
						tickmarksEnabled: true
						stepSize: 0.4
						minimumValue: 0
						maximumValue: 4
						value: 0.8
					}
				}
				Rectangle {
					width: 30
					height: parent.height
					color: "transparent"
					Label {
						anchors.verticalCenter: parent.verticalCenter
						text: vect_reducPuntos.value
					}
				}
			}

			Root.TabFormItem {
				text: "Suavizado:"
				Rectangle {
					height: parent.height - 12
					width: 100

					Slider {
						id: vect_suavizado
						anchors.bottom: parent.bottom
						implicitWidth: parent.width
						tickmarksEnabled: true
						stepSize: 1
						minimumValue: 0
						maximumValue: 5
						value: 1
					}
				}
				Label {
					width: 30
					text: vect_suavizado.value
				}
			}

			Root.TabSeparator {	}

			Button {
				id: vec_btVectorizar
				text: "Vectorizar..."
				implicitHeight: 28
				implicitWidth: 96
				iconSource: "qrc:/img/vector.png"
				anchors.verticalCenter: parent.verticalCenter

				onClicked: dialGuardarVect.open()
			}
		}
	}


	// Área del mapa
	Mapa.AreaMapa {
		id: mapa
		width: parent.width
		anchors.top: barraTabs.bottom
		anchors.bottom: parent.bottom

		onMapaClick: LogMain.mapaClick(mouse)
		onMouseMovimiento: {
			if (marca4.tieneDatos) {
				var p = iMapa.getCoords(mouse.x / escala, mouse.y / escala)
				lblCoords.text = p.x + " E, " + p.y + " N"
			}
		}

		// Marcas para recorte
		Etapas.Marca { id: marca1; num: 0; escala: mapa.escala }
		Etapas.Marca { id: marca2; num: 1; escala: mapa.escala }
		Etapas.Marca { id: marca3; num: 2; escala: mapa.escala }
		Etapas.Marca { id: marca4; num: 3; escala: mapa.escala }

		// Área de recorte
		Etapas.ResizeRect {
			id: rectRecorte
			visible: false

			onClickDer: {
				LogEtaCorte.recortarImagen()
				btRecortar.checked = false
				root.actividad = 0
			}
		}
	}


	// Barra de estado
	statusBar: StatusBar {
		id: barraEstado
		property alias texto: lblTexto.text

		RowLayout {
			Rectangle {
				width: 150
				height: 16
				color: "transparent"

				Label {
					id: lblCoords
					text: "(sin georreferenciación)"
					y: 2
				}
				Rectangle {
					anchors.right: parent.right
					width: 1
					height: 16
					color: "#AAAAAA"
				}
			}

			ProgressBar {
				id: barraOcupado
				indeterminate: true
				implicitHeight: 16
				implicitWidth: 300
				visible: false

				Label {
					text: "Trabajando. Por favor espere..."
					y: 1
					x: 6
				}
			}
			Label {
				id: lblTexto
				text: ""
			}

		}

		ToolButton {
			anchors.right: zoomSB.left
			anchors.rightMargin: 10
			iconSource: "qrc:/img/capa-sec.png"
			tooltip: "Mostrar capa secundaria"
			checkable: true
			checked: true
			implicitHeight: 17
			implicitWidth: 17

			onClicked: {
				if (checked) {
					mapa.mostrarTrazos()
				}
				else {
					mapa.ocultarTrazos()
				}
			}
		}

		Item {
			id: zoomSB
			width: 164
			anchors.right: parent.right

			Text {
				id: zoom_txt
				width: 30
				smooth: false
				verticalAlignment: Text.AlignVCenter
				horizontalAlignment: Text.AlignRight
				lineHeight: 22
				text: (zoom_slider.value >= 0 ? (zoom_slider.value + 1) * 100 : Math.floor(100 / (1 - zoom_slider.value))) + "%"
			}
			Row {
				x: 35
				spacing: 8

				Root.BtRedondo {
					iconSource: "qrc:/img/minus.png"
					onClicked: mapa.zoomOut()
				}
				Slider {
					id: zoom_slider
					implicitWidth: 80
					implicitHeight: 16
					stepSize: 1
					minimumValue: -4
					maximumValue: 5
					value: 0
					onValueChanged: {
						if (pressed) {
							if (value < mapa.numEscala) mapa.zoomOut()
							else mapa.zoomIn()
						}
					}
				}
				Root.BtRedondo {
					iconSource: "qrc:/img/plus.png"
					onClicked: mapa.zoomIn()
				}
			}
		}
	}
}
