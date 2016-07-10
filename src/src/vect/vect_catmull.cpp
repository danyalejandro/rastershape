#include "vect_catmull.h"

namespace catmull {
	/// <summary>
	/// Generate spline series of points from array of keyframe points
	/// </summary>
	/// <param name="points">array of key frame points</param>
	/// <param name="numPoints">number of points to generate in spline between each point</param>
	/// <returns>array of points describing spline</returns>
	void interpolar(vector<Point2f> &points, vector<Point2f> &splinePoints, int numNewPoints) {
		int i, j, numPunOri;
		
		if (numNewPoints < 1) return;
		
		numPunOri = points.size();
		splinePoints.clear();
		
		if (numPunOri < 4) {
			for (i = 0 ; i < numPunOri ; i++) splinePoints.push_back(points[i]);
		}
		else {
			splinePoints.push_back(points[0]);
			for (i = 0 ; i < numPunOri - 3 ; i++) {
				for (j = 0 ; j < numNewPoints ; j++) {
					splinePoints.push_back(
						PointOnCurve(points[i], points[i + 1], points[i + 2], points[i + 3], ((float)1 / numNewPoints) * j)
					);
				}
			}
			splinePoints.push_back(points[numPunOri - 2]);
			splinePoints.push_back(points[numPunOri - 1]);
		}
	}
	
	
	/// <summary>
	/// Calculates interpolated point between two points using Catmull-Rom Spline
	/// </summary>
	/// <remarks>
	/// Points calculated exist on the spline between points two and three.
	/// </remarks>
	/// <param name="p0">First Point</param>
	/// <param name="p1">Second Point</param>
	/// <param name="p2">Third Point</param>
	/// <param name="p3">Fourth Point</param>
	/// <param name="t">
	/// Normalised distance between second and third point
	/// where the spline point will be calculated
	/// </param>
	/// <returns>
	/// Calculated Spline Point
	/// </returns>
	Point2f PointOnCurve(Point2f p0, Point2f p1, Point2f p2, Point2f p3, float t) {
		Point2f ret;
		float t2 = t * t;
		float t3 = t2 * t;
		ret.x = 0.5f * ((2.0f * p1.x) +	(-p0.x + p2.x) * t + (2.0f * p0.x - 5.0f * p1.x + 4 * p2.x - p3.x) * t2 +
				(-p0.x + 3.0f * p1.x - 3.0f * p2.x + p3.x) * t3);
		ret.y = 0.5f * ((2.0f * p1.y) + (-p0.y + p2.y) * t + (2.0f * p0.y - 5.0f * p1.y + 4 * p2.y - p3.y) * t2 +
				(-p0.y + 3.0f * p1.y - 3.0f * p2.y + p3.y) * t3);
		return ret;
	}
}