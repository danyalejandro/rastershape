/* 
 * File:   Recta.h
 * Author: DanyAlejandro
 *
 * Created on 15 de agosto de 2014, 09:22 AM
 */

#ifndef RECTA_H
#define	RECTA_H

#include "opencv2/core/core.hpp"

using namespace cv;

// Almacena los parámetros de la ecuación de una recta
struct Recta {
	Point2f puntoIni;
	Point2f puntoFin;
	float longitud;
};

#endif	/* RECTA_H */

