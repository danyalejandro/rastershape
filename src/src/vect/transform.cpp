#include "transform.h"
#include "src/infomapa.h"

transformGeo::transformGeo()
    {

        CA = 0;
        CB = 0;
        CC = 0;
        CD = 0;
        CE = 0;
        CF = 0;


    }

 void transformGeo::setCoeficients(InfoMapa *iMapa)
    {
        double n = 4;
        double sx = iMapa->aX + iMapa->bX + iMapa->cX + iMapa->dX;
        double sy = iMapa->aY + iMapa->bY + iMapa->cY + iMapa->dY;
        double sx2 = (iMapa->aX*iMapa->aX) + (iMapa->bX*iMapa->bX) + (iMapa->cX*iMapa->cX) + (iMapa->dX*iMapa->dX);
        double sxy = (iMapa->aX*iMapa->aY)+(iMapa->bX*iMapa->bY)+(iMapa->cX*iMapa->cY)+(iMapa->dX*iMapa->dY);
        double sy2 = (iMapa->aY*iMapa->aY) + (iMapa->bY*iMapa->bY) + (iMapa->cY*iMapa->cY) + (iMapa->dY*iMapa->dY);

        double SX = iMapa->aE+iMapa->bE+iMapa->cE+iMapa->dE;
        double SY = iMapa->aN+iMapa->bN+iMapa->cN+iMapa->dN;
        double SxX = (iMapa->aE*iMapa->aX)+(iMapa->bE*iMapa->bX)+(iMapa->cE*iMapa->cX)+(iMapa->dE*iMapa->dX);
        double SxY = (iMapa->aN*iMapa->aX)+(iMapa->bN*iMapa->bX)+(iMapa->cN*iMapa->cX)+(iMapa->dN*iMapa->dX);
        double SyX = (iMapa->aE*iMapa->aY)+(iMapa->bE*iMapa->bY)+(iMapa->cE*iMapa->cY)+(iMapa->dE*iMapa->dY);
        double SyY = (iMapa->aN*iMapa->aY)+(iMapa->bN*iMapa->bY)+(iMapa->cN*iMapa->cY)+(iMapa->dN*iMapa->dY);
        getCoefficients(n,sx, sy, sx, sx2, sxy, sy, sxy, sy2, SX, SY, SxX, SxY, SyX, SyY);
    }

    double transformGeo::getDeterminant2(double a, double b, double c, double d){
        return (a*d) - (b*c);
    }
    double transformGeo::getDeterminant3(){
        double a11 = A[0][0];
        double a12 = A[0][1];
        double a13 = A[0][2];
        double a21 = A[1][0];
        double a22 = A[1][1];
        double a23 = A[1][2];
        double a31 = A[2][0];
        double a32 = A[2][1];
        double a33 = A[2][2];
       /* cout<<a11<<"++++"<<endl;
        cout<<a12<<"++++"<<endl;
        cout<<a13<<"++++"<<endl;
        cout<<a21<<"++++"<<endl;
        cout<<a22<<"++++"<<endl;
        cout<<a23<<"++++"<<endl;
        cout<<a31<<"++++"<<endl;
        cout<<a32<<"++++"<<endl;
        cout<<a33<<"++++"<<endl;*/
        return a11*a22*a33 + a12*a23*a31 + a13*a21*a32 - a13*a22*a31 - a12*a21*a33 - a11*a23*a32;

    }

    void transformGeo::getInverse(){

        double a11 = A[0][0];
        double a12 = A[0][1];
        double a13 = A[0][2];
        double a21 = A[1][0];
        double a22 = A[1][1];
        double a23 = A[1][2];
        double a31 = A[2][0];
        double a32 = A[2][1];
        double a33 = A[2][2];
        double determinant = getDeterminant3();

        cout<<"*********************"<<determinant<<endl;
        if(determinant != 0.0)
        {
                    B[0][0] =  getDeterminant2(a22, a23, a32, a33)/determinant;
                    B[0][1] =  getDeterminant2(a13, a12, a33, a32)/determinant;
                    B[0][2] =  getDeterminant2(a12, a13, a22, a23)/determinant;
                    B[1][0] =  getDeterminant2(a23, a21, a33, a31)/determinant;
                    B[1][1] =  getDeterminant2(a11, a13, a31, a33)/determinant;
                    B[1][2] =  getDeterminant2(a13, a11, a23, a21)/determinant;
                    B[2][0] =  getDeterminant2(a21, a22, a31, a32)/determinant;
                    B[2][1] =  getDeterminant2(a12, a11, a32, a31)/determinant;
                    B[2][2] =  getDeterminant2(a11, a12, a21, a22)/determinant;

                    /*cout<<B[0][0]<<"}}}}}}}}}}}}}}"<<endl;
                    cout<<B[1][0]<<"}}}}}}}}}}}}}}"<<endl;
                    cout<<B[0][1]<<"}}}}}}}}}}}}}}"<<endl;
                    cout<<B[0][2]<<"}}}}}}}}}}}}}}"<<endl;
                    cout<<B[1][1]<<"}}}}}}}}}}}}}}"<<endl;
                    cout<<B[1][2]<<"}}}}}}}}}}}}}}"<<endl;
                    cout<<B[2][0]<<"}}}}}}}}}}}}}}"<<endl;
                    cout<<B[2][1]<<"}}}}}}}}}}}}}}"<<endl;
                    cout<<B[2][2]<<"}}}}}}}}}}}}}}"<<endl;*/

        }


    }
    double transformGeo::getX(int x, int y){
        return (CA*x)  + (CB*y) + CC;
    }

    double transformGeo::getY(int x, int y){
        return (CD*x) + (CE*y) + CF;
    }

    void transformGeo::getCoefficients(double a11, double a12, double a13, double a21, double a22, double a23, double a31, double a32, double a33, double c11, double c12, double c21, double c22, double c31, double c32)
    {
        A[0][0] = a11;
        A[0][1] = a12;
        A[0][2] = a13;
        A[1][0] = a21;
        A[1][1] = a22;
        A[1][2] = a23;
        A[2][0] = a31;
        A[2][1] = a32;
        A[2][2] = a33;

        getInverse();

        double b11 = B[0][0];
        double b12 = B[0][1];
        double b13 = B[0][2];
        double b21 = B[1][0];
        double b22 = B[1][1];
        double b23 = B[1][2];
        double b31 = B[2][0];
        double b32 = B[2][1];
        double b33 = B[2][2];

        cout<<b11<<"++++"<<endl;
        cout<<b12<<"++++"<<endl;
        cout<<b13<<"++++"<<endl;
        cout<<b21<<"++++"<<endl;
                cout<<b22<<"++++"<<endl;
                cout<<b23<<"++++"<<endl;
                cout<<b31<<"++++"<<endl;
                cout<<b32<<"++++"<<endl;
                cout<<b33<<"++++"<<endl;

        double d11 = (b11*c11) + (b12*c21) + (b13*c31);
        double d12 = (b11*c12) + (b12*c22) + (b13*c32);

        double d21 = (b21*c11) + (b22*c21) + (b23*c31);
        double d22 = (b21*c12) + (b22*c22) + (b23*c32);

        double d31 = (b31*c11) + (b32*c21) + (b33*c31);
        double d32 = (b31*c12) + (b32*c22) + (b33*c32);

        CC = d11;
        CF = d12;

        CA = d21;
        CD = d22;

        CB = d31;
        CE = d32;

    }

    double transformGeo::getCC()
    {
        return CC;
    }

    double transformGeo::getCD()
    {
        return CD;
    }

    double transformGeo::getCB()
    {
        return CB;
    }

    double transformGeo::getCA()
    {
        return CA;
    }

    double transformGeo::getCE()
    {
        return CE;
    }

    double transformGeo::getCF()
    {
        return CF;
    }





