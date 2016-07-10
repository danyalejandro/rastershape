#ifndef VECT_H
#define	VECT_H

#include <iostream>

#include "opencv2/core/core.hpp"
#include "shapelib/shapefil.h"
#include "opencv2/imgproc/imgproc.hpp"
#include "vect_catmull.h"
#include "vect_RDP.h"
#include "transform.h"
#include "src/infomapa.h"

using namespace cv;
using namespace std;

namespace vect {
	void *vecindad(Mat &img, Point pun, uchar vec[]);
	uchar codVecindad(Mat &img, Point pun);
	bool esPunFinal(Mat &img, Point pun);
	void puntosFinales(Mat &img, vector<Point> &punFinales);
	void moverPuntoCod(Point &pun, uchar cod);
	SHPObject *toArc(vector<Point2f> vecPuntosTrazo, int imgHeight);
    void agregarShape(vector<Point2f> trazo, int altura, SHPHandle sh, transformGeo *objGeo, Point pA, DBFHandle dbf);
	void moverPunto(Point &punto, uchar dir);
	void recorrerTrazo(Mat &imgBase, Point pun, vector<Point2f> &trazo, bool trazoCerrado);
	
    void execVectorize(Mat &imgorigi, const char *filename, float deltaRDP, short int suavizado, InfoMapa *iMapa);
};

#endif	/* VECT_H */

