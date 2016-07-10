#ifndef LINERECONSTRUCTION_H
#define LINERECONSTRUCTION_H

#include <iostream>
#include <math.h>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>


using namespace cv;
using namespace std;

namespace reco {
    int isValid(int j, int i, Mat& src1, int x, int y);
    double dist(int x0, int x1, int y0, int y1);
    char* parsePoint(int x, int y);

    bool isEndPoint(int);
    int getCode(uchar, uchar,uchar,uchar,uchar,uchar,uchar,uchar);
    Point nextPoint(int, int , int , Mat);
    int getDir(int, int , Mat);
    double dirEndPoint(int, int , int, Mat);
    int changeDir(int);
    bool deletePoints(int, int);
    double getChainDir(int);
    int getCodeDir(double);
    Point findPoint(int, int , int , Mat);
    int execLineReconstruction(Mat&, int);
    void paintLine(Point, Point, Mat);
}
#endif // LINERECONSTRUCTION_H
