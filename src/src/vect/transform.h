#ifndef TRANSFORM_H
#define TRANSFORM_H


#include <iostream>
#include "src/infomapa.h"

using namespace std;

class transformGeo{

private:
    double A[3][3];
    double B[3][3];
    double C[3][2];
    double D[3][2];
    double CA, CB, CC, CD, CE, CF;

    double getDeterminant3();
    double getDeterminant2(double, double, double, double);
    void getInverse();

public:
    double getCA();
    double getCB();
    double getCC();
    double getCD();
    double getCE();
    double getCF();
    transformGeo();
    void setCoeficients(InfoMapa *iMapa);
    //transformGeo(InfoMapa *iMapa);
    void getCoefficients(double, double, double, double, double, double, double, double, double, double, double, double, double, double, double);
    double getX(int, int);
    double getY(int, int);
};


#endif // TRANSFORM_H
