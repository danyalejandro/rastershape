#ifndef UTILES_H
#define	UTILES_H

#include <iostream>
#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

#define M_PI 3.14159265358979323846
#define M_PI_2 1.570796327
#define M_PI_8 0.392699 // pi/8

using namespace cv;
using namespace std;

/* Almacena la información de un cuerpo de pixeles */
struct CuerpoPix {
    Rect mbr; // Rectángulo contenedor con los atributos: x, y, width, height
    Point pIni; // Primer punto recorrido en el cuerpo
    Point pFin; // Último punto recorrido

    Point pIzq; // Punto del cuerpo en el xMin (izquierda)
    Point pDer; // Punto del cuerpo en el xMax (derecha)
    Point pArr; // Punto del cuerpo en el yMin (arriba)
    Point pAba; // Punto del cuerpo en el yMax (abajo)
};

void getCuerpoPix(Point punIni, CuerpoPix &cuerpo, Mat &img, Mat &imgMarcas, int numPixMax, uchar numOrden = 8, uchar numVecinosCola = 8);

void flecha(Mat &img, float xi, float yi, float mag, float angulo, Scalar color);

float rad2deg(float angulo);
uchar rad2dir(float ang);
uchar dirOpuesta(uchar dir);
float normalizar(float a);
float anguloPositivo(float a);
float anguloOpuesto(float a);
float anguloPunto(Point2f p);

Point moverPosicion(Point punto, uchar dir);
void angulo2vector(float a, Point2f &p);
float anguloDifPuntos(Point2f p1, Point2f p2);
float restarAngulos(float a1, float a2);
float distanciaPuntos(Point2f p1, Point2f p2);

float alturaTriangulo(Point2f A, Point2f B, Point2f C);

uchar *vecindad(Mat &img, Point pun, uchar vec[]);

#endif	/* UTILES_H */

