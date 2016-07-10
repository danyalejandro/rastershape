/* 
 * File:   reduc.h
 * Author: DanyAlejandro
 *
 * Created on 5 de septiembre de 2014, 11:39 AM
 */

#ifndef REDUC_H
#define	REDUC_H

#include <math.h>
#include <vector>
#include <iostream>
#include <string>
#include "opencv2/core/core.hpp"

using namespace std;
using namespace cv;

namespace vec_RDP {
	class RamerDouglasPeuckerPoint {
		public:
			virtual double getRamerDouglasPeuckerDistance(RamerDouglasPeuckerPoint &p1, RamerDouglasPeuckerPoint &p2) = 0;
			virtual void markAsDiscarded() = 0;
	};

	class MapPoint : public RamerDouglasPeuckerPoint {
		protected:
			double x;
			double y;
			bool discarded;
		public:
			MapPoint(double x, double y);
			bool isEqualTo(MapPoint &p);
			double getRamerDouglasPeuckerDistance(RamerDouglasPeuckerPoint &p1, RamerDouglasPeuckerPoint &p2);
			void markAsDiscarded();
			double getX();
			double getY();
			bool isDiscarded();
	};
	
	template <class T> void ramerDouglasPeuckerSimplify(string prefix, vector<T> &v, double epsilon, int left = -1, int right = -1);
	
	void reducirPuntos(vector<Point2f> &vecOrigi, float deltaRDP);
};

#endif	/* REDUC_H */

