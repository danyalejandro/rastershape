import QtQuick 2.3
import QtQuick.Layouts 1.1
import QtQuick.Controls 1.2


RowLayout {
	property alias text: etiqueta.text
	height: parent.height

	Label {
		id: etiqueta
		text: "Label:"
		smooth: false
		anchors.rightMargin: 5
	}
}
