/* 
 * File:   vect_catmull.h
 * Author: DanyAlejandro
 *
 * Created on 5 de septiembre de 2014, 08:45 PM
 */

#ifndef VECT_CATMULL_H
#define	VECT_CATMULL_H

#include <iostream>
#include "opencv2/core/core.hpp"

using namespace cv;
using namespace std;

namespace catmull {
	void interpolar(vector<Point2f> &points, vector<Point2f> &splinePoints, int numPoints);
	Point2f PointOnCurve(Point2f p0, Point2f p1, Point2f p2, Point2f p3, float t);
}

#endif	/* VECT_CATMULL_H */

