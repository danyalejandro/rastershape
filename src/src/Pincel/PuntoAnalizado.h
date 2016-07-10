/* 
 * File:   PuntoAnalizado.h
 * Author: DanyAlejandro
 *
 * Created on 2 de septiembre de 2014, 10:48 AM
 */

#ifndef PUNTOANALIZADO_H
#define	PUNTOANALIZADO_H

#include <iostream>
#include "opencv2/core/core.hpp"
#include "utileria.h"

using namespace cv;
using namespace std;

// Almacena la información de un punto analizado
struct PuntoAnalizado {
    Point pos;
    float angulo; // Ángulo desde 0° medido con punPincel como origen
	uchar codValido; // 0: valido, 1: ángulo inválido, 2: distGuia inválida
	unsigned int num; // Número de análisis, viene del orden de recorrido
};

ostream& operator << (ostream &o, const PuntoAnalizado &pun);

#endif	/* PUNTOANALIZADO_H */

