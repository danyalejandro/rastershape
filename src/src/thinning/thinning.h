#ifndef THINNING_H
#define THINNING_H

#include <iostream>

#include "opencv2/core/core.hpp"
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

using namespace cv;
using namespace std;

void applyGaussianBlur(cv::Mat&, int, int, int);

void thinningGuoHallIteration(cv::Mat&, int);
void thinningGuoHall(cv::Mat&);
void execThinningGuoHall(cv::Mat&);

void thinningIterationZhang(cv::Mat&, int);
void thinningZhang(cv::Mat&);
void execThinningZhang(cv::Mat&,cv::Mat&);

#endif // THINNING_H
