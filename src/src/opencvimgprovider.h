#ifndef OPENCVIMGPROVIDER_H
#define OPENCVIMGPROVIDER_H

#include <QQuickImageProvider>
#include <QPainter>

#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "Mat2Qt.h"
#include <iostream>

using namespace cv;
using namespace std;

class OpencvImgProvider : public QQuickImageProvider {
private:

    QPixmap getImgBinaria(int xMin, int yMin, int xMax, int yMax, int ancho, int alto);
    QPixmap getImgTrazos(int xMin, int yMin, int xMax, int yMax, int ancho, int alto);
    QPixmap getImgPincel(int xMin, int yMin, int xMax, int yMax, int ancho, int alto);

public:
    Mat imgBinaria;
    Mat imgTrazo;
    Mat imgPincel;

    OpencvImgProvider();
    QPixmap requestPixmap(const QString &id, QSize *size, const QSize &requestedSize);
    //QImage requestImage(const QString & id, QSize * size, const QSize & requestedSize);
};

#endif // OPENCVIMGPROVIDER_H
