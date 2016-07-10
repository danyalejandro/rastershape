#include "vect_RDP.h"

vec_RDP::MapPoint::MapPoint(double x, double y) {
	this->x = x;
	this->y = y;
	this->discarded = false;
}

bool vec_RDP::MapPoint::isEqualTo(MapPoint &p) {
	return ((this->x == p.getX()) && (this->y == p.getY()));
}

double vec_RDP::MapPoint::getRamerDouglasPeuckerDistance(RamerDouglasPeuckerPoint &p1, RamerDouglasPeuckerPoint &p2) {
	double dx, dy, ret;
	MapPoint *aux1 = dynamic_cast<MapPoint *>(&p1);
	MapPoint *aux2 = dynamic_cast<MapPoint *>(&p2);
	if (aux1->isEqualTo(*aux2)) {
		double dx = aux1->getX() - this->x;
		double dy = aux1->getY() - this->y;
		ret = sqrt((dx * dx) + (dy * dy));
	}
	else {
		// first calculate the director vector
		dx = aux2->getX() - aux1->getX();
		dy = aux2->getY() - aux1->getY();
		// now calculate the implicit equation: Ax + By + C = 0
		// the director vector (dx, dy) = (-B, A)
		double a = dy;
		double b = -dx;
		double c = -(a * aux1->getX()) - (b * aux1->getY());
		//cout << "a=" << a << " b=" << b << " c=" << c << endl;
		// and finally, calculate distance between "this" and Ax + By + C = 0
		ret = fabs((a * this->x) + (b * this->y) + c) / sqrt((a * a) + (b * b));
	}
	//cout << "distance = " << ret << endl;
	return ret;
}

void vec_RDP::MapPoint::markAsDiscarded() {
	this->discarded = true;
}

double vec_RDP::MapPoint::getX() {
	return this->x;
}

double vec_RDP::MapPoint::getY() {
	return this->y;
}

bool vec_RDP::MapPoint::isDiscarded() {
	return this->discarded;
}

template <class T> void vec_RDP::ramerDouglasPeuckerSimplify(string prefix, vector<T> &v, double epsilon, int left , int right) {
	if (left < 0) left = 0;
	if (right < 0) right = v.size() - 1;
	//cout << prefix << "ramerDouglasPeuckerSimplify: [" << left << ", " << right << "]" << endl;
	// find the point with the max distance
	int index = 0;
	double maxDistance = 0;
	for (int i = left + 1; i <= (right - 1); i++) {
		RamerDouglasPeuckerPoint *p = &v[i];
		double d = p->getRamerDouglasPeuckerDistance(v[left], v[right]);
		if (d > maxDistance) {
			index = i;
			maxDistance = d;
		}
	}
	//cout << prefix << "maxDistance=" << maxDistance << " at index=" << index << " (epsilon=" << epsilon << ")" << endl;
	if (maxDistance > epsilon) {
		// recursive
		ramerDouglasPeuckerSimplify(prefix + "\t", v, epsilon, left, index);
		ramerDouglasPeuckerSimplify(prefix + "\t", v, epsilon, index, right);
	}
	else {
		// discard all but first and last point
		for (int i = left + 1; i <= (right - 1); i++) {
			v[i].markAsDiscarded();
			//cout << prefix << "index " << i << " discarded" << endl;
		}
	}
}

// Aplica la reducción de puntos al vecOrigi
void vec_RDP::reducirPuntos(vector<Point2f> &vecOrigi, float deltaRDP) {
	if (vecOrigi.size() < 3) return;
	
	// convertimos el vector de Point en MapPoint para aplicar RDPS
	vector<MapPoint> vecMapPoint;
	for(vector<Point2f>::iterator it = vecOrigi.begin(); it != vecOrigi.end(); ++it) {
		vecMapPoint.push_back(MapPoint((*it).x, (*it).y));
	}
	
	ramerDouglasPeuckerSimplify<MapPoint>("", vecMapPoint, deltaRDP, -1, -1);
	
	vecOrigi.clear();
	for(vector<MapPoint>::iterator it = vecMapPoint.begin(); it != vecMapPoint.end(); ++it) {		
		if (!(*it).isDiscarded())
			vecOrigi.push_back(Point2f((*it).getX(), (*it).getY()));
	}
}