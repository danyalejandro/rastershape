#include "thinning.h"
#include <exception>

void applyGaussianBlur(Mat& src, int size, int desvX, int desvY) {
    GaussianBlur(src, src, Size(3, 3), 0);
    GaussianBlur(src, src, Size(3, 3), 0);
    GaussianBlur(src, src, Size(5, 5), 0);
    GaussianBlur(src, src, Size(11, 11), 0);
}

void thinningGuoHallIteration(Mat& img, int iter) {
    cout << img.size();
    Mat marker = Mat::zeros(img.size(), CV_8UC1);
    int i, j, C, N1, N2, N, m;
    uchar p2, p3, p4, p5, p6, p7, p8, p9;

    for ( i = 1 ; i < img.rows - 1 ; i++) {
        for ( j = 1 ; j < img.cols - 1 ; j++) {
            p2 = img.at<uchar>(i-1, j);
            p3 = img.at<uchar>(i-1, j+1);
            p4 = img.at<uchar>(i, j+1);
            p5 = img.at<uchar>(i+1, j+1);
            p6 = img.at<uchar>(i+1, j);
            p7 = img.at<uchar>(i+1, j-1);
            p8 = img.at<uchar>(i, j-1);
            p9 = img.at<uchar>(i-1, j-1);

            C  = (!p2 & (p3 | p4)) + (!p4 & (p5 | p6)) +
                     (!p6 & (p7 | p8)) + (!p8 & (p9 | p2));
            N1 = (p9 | p2) + (p3 | p4) + (p5 | p6) + (p7 | p8);
            N2 = (p2 | p3) + (p4 | p5) + (p6 | p7) + (p8 | p9);
            N  = N1 < N2 ? N1 : N2;
            m  = iter == 0 ? ((p6 | p7 | !p9) & p8) : ((p2 | p3 | !p5) & p4);

            if (C == 1 && (N >= 2 && N <= 3) & m == 0)
                marker.at<uchar>(i,j) = 1;
        }
    }

    img &= ~marker;
}

/**
 * Function for thinning the given binary image
 *
 * @param  im  Binary image with range = 0-255
 */
void thinningGuoHall(Mat& im) {
    Mat prev = Mat::zeros(im.size(), CV_8UC1);
    Mat diff;

    do {
        thinningGuoHallIteration(im, 0);
        thinningGuoHallIteration(im, 1);
        absdiff(im, prev, diff);
        im.copyTo(prev);
    } while (countNonZero(diff) > 0);
}


void execThinningGuoHall(Mat& src) {
    if (src.empty()) return;

    // Al aplicar desenfoque, pueden aparecer pixeles no blancos en los bordes, por
    // lo que forzamos los bordes a ser blancos
    src.col(0).setTo(Scalar(255));
    src.col(src.cols - 1).setTo(Scalar(255));
    src.row(0).setTo(Scalar(255));
    src.row(src.rows - 1).setTo(Scalar(255));


    threshold(src, src, 220, 1, THRESH_BINARY_INV);

    thinningGuoHall(src);

    // Invertimos de nuevo
    threshold(src, src, 0, 255, THRESH_BINARY_INV);
}


void thinningIterationZhang(Mat& im, int iter) {
    Mat marker = Mat::zeros(im.size(), CV_8UC1);

    for (int i = 1; i < im.rows-1; i++) {
        for (int j = 1; j < im.cols-1; j++) {
            uchar p2 = im.at<uchar>(i-1, j);
            uchar p3 = im.at<uchar>(i-1, j+1);
            uchar p4 = im.at<uchar>(i, j+1);
            uchar p5 = im.at<uchar>(i+1, j+1);
            uchar p6 = im.at<uchar>(i+1, j);
            uchar p7 = im.at<uchar>(i+1, j-1);
            uchar p8 = im.at<uchar>(i, j-1);
            uchar p9 = im.at<uchar>(i-1, j-1);

            int A  = (p2 == 0 && p3 == 1) + (p3 == 0 && p4 == 1) +
                     (p4 == 0 && p5 == 1) + (p5 == 0 && p6 == 1) +
                     (p6 == 0 && p7 == 1) + (p7 == 0 && p8 == 1) +
                     (p8 == 0 && p9 == 1) + (p9 == 0 && p2 == 1);
            int B  = p2 + p3 + p4 + p5 + p6 + p7 + p8 + p9;
            int m1 = iter == 0 ? (p2 * p4 * p6) : (p2 * p4 * p8);
            int m2 = iter == 0 ? (p4 * p6 * p8) : (p2 * p6 * p8);

            if (A == 1 && (B >= 2 && B <= 6) && m1 == 0 && m2 == 0)
                marker.at<uchar>(i,j) = 1;
        }
    }

    im &= ~marker;
}

void thinningZhang(Mat& im)
{
    im /= 255;

    Mat prev = Mat::zeros(im.size(), CV_8UC1);
    Mat diff;

    do {
        thinningIterationZhang(im, 0);
        thinningIterationZhang(im, 1);
        absdiff(im, prev, diff);
        im.copyTo(prev);
    }
    while (countNonZero(diff) > 0);

    im *= 255;
}

void execThinningZhang(Mat& src, Mat& dst )
{
    if (src.empty()) return;
    bitwise_xor(src, Scalar(255, 255, 255), dst);
    //imwrite("c:/img/bw.png", dst);
    //Mat bw;
    cvtColor(dst, dst, CV_BGR2GRAY);
    thinningZhang(dst);
    bitwise_xor(dst, Scalar(255), dst);

}
