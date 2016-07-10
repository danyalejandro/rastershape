#include "Pincel.h"

// Inicializa la imagen de depuración
void Pincel::initMatDebug() {
	namedWindow("Debug", WINDOW_NORMAL | CV_GUI_NORMAL);
	// Duplicamos la imagen de base en el debug
	int y, x;
	uchar* fila;
	uchar intensidad;
	int imgAlto = imgBase.rows;
	int imgAncho = imgBase.cols;
	imgDebug = Mat(imgAlto, imgAncho, CV_8UC3, Scalar(255, 255, 255));

	for (y = 0 ; y < (imgAlto - 1) ; ++y) {
		fila = imgBase.ptr<uchar>(y);
		for (x = 0 ; x < (imgAncho - 1) ; ++x) {
			intensidad = fila[x];
			if (intensidad == 0) {
				imgDebug.at<Vec3b>(y, x) = Vec3b(0, 0, 0);
			}
		}
	}
}

void Pincel::marcaTrazoPincel(Point punto) {
    imgPincelTrazo.at<uchar>(punto.y, punto.x) = 0;
}

// Marca un punto en la matriz de recorrido
void Pincel::marcaRecorrido(Point punto) {
	imgRecorrido.at<uchar>(punto.y, punto.x) = 0;
}

// Desmarca un punto en la matriz de recorrido
void Pincel::desmarcaPuntoRecorrido(Point punto) {
	imgRecorrido.at<uchar>(punto.y, punto.x) = 255;
}

// Marca un punto de cierto color en la matriz de debug
// Colores disponibles: 0: rojo, 1:naranja, 2:amarillo, 3:verde, 4:cyan, 5:azul, 6:morado
void Pincel::marcaDebug(Point punto, uchar nColor) {
	Vec3b color;
	color = imgDebug.at<Vec3b>(punto.y, punto.x);
	
	switch (nColor) {
		case 0: color = Vec3b(180,150,150); break;
		case 1: color = Vec3b(20,0,220); break;
		case 2: color = Vec3b(50,150,255); break;
		case 3: color = Vec3b(0,255,255); break;
		case 4: color = Vec3b(0,255,0); break;
		case 5: color = Vec3b(255,255,0); break;
		case 6: color = Vec3b(255,120,90); break;
		case 7: color = Vec3b(255,0,255); break;

	}
	imgDebug.at<Vec3b>(punto.y, punto.x) = color;
}



void Pincel::mostrarRecorrido(bool ampliar) {
	if (ampliar) {
		Mat mGrande;
		resize(imgRecorrido, mGrande, Size(imgRecorrido.cols * 2, imgRecorrido.rows * 2), 0, 0, INTER_NEAREST);
		imshow("Recorrido", mGrande);
	}
	else {
		imshow("Recorrido", imgRecorrido);
	}
	//waitKey(0);
}

void Pincel::mostrarDebug(bool ampliar) {
	if (ampliar) {
		Mat mGrande;
		short int escala = 1; // proporción
		int resX = 1024, resY = 768 - 32;
		
		do { escala++; }
		while (imgDebug.cols * escala <= resX && imgDebug.rows * escala <= resY);
		escala--;
		
		resize(imgDebug, mGrande, Size(imgDebug.cols * escala, imgDebug.rows * escala), 0, 0, INTER_NEAREST);
		
		circle(mGrande, puntoPincel * escala, radio * escala, Scalar(0,240,0)); // Agregamos el circulo del pincel
		flecha(mGrande, puntoPincel.x * escala, puntoPincel.y * escala, 15, anguloEstim, Scalar(0,240,0));
		line(mGrande, Point2f(rectaGuia.puntoIni.x, rectaGuia.puntoIni.y) * escala, Point2f(rectaGuia.puntoFin.x, rectaGuia.puntoFin.y) * escala, Scalar(250,0,255));
		
		//debugVideo << mGrande;
		
		imshow("Debug", mGrande);
		//mostrarRecorrido(false);
	}
	else {
		imshow("Debug", imgDebug);
	}
	
	waitKey(0);
}

void Pincel::guardarDebug() {
	imwrite("debug.bmp", imgDebug);
}