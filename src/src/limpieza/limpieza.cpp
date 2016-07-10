#include <iostream>
#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

#include "limpieza.h"

using namespace cv;
using namespace std;

int numAislados1 = 0;
int numAislados2 = 0;

// Devuelve el código de la vecindad de un pixel
uchar limpia::getCodVecindad(Mat &img, int x, int y) {
    uchar vecindad[7];
    // iniciamos arriba y rotamos en sentido horario
    vecindad[0] = (img.at<uchar>(y-1,	x) == 0) ? 1 : 0;
    vecindad[1] = (img.at<uchar>(y-1,	x+1) == 0) ? 1 : 0;
    vecindad[2] = (img.at<uchar>(y,		x+1) == 0) ? 1 : 0;
    vecindad[3] = (img.at<uchar>(y+1,	x+1) == 0) ? 1 : 0;
    vecindad[4] = (img.at<uchar>(y+1,	x) == 0) ? 1 : 0;
    vecindad[5] = (img.at<uchar>(y+1,	x-1) == 0) ? 1 : 0;
    vecindad[6] = (img.at<uchar>(y,		x-1) == 0) ? 1 : 0;
    vecindad[7] = (img.at<uchar>(y-1,	x-1) == 0) ? 1 : 0;

    // aplicamos codificación Greenlee (1987)
    uchar codVecindad = 128 * int(vecindad[0]) + int(vecindad[1]) + 2 * int(vecindad[2])
            + 4 * int(vecindad[3]) + 8 * int(vecindad[4]) + 16 * int(vecindad[5])
            + 32 * int(vecindad[6]) + 64 * int(vecindad[7]);
    return codVecindad;
}

// true si es el código greenlee de un punto final de línea
bool limpia::codPuntoFinal(uchar codVecindad) {
    return (codVecindad == 1 || codVecindad == 2 || codVecindad == 4 || codVecindad == 8
            || codVecindad == 16 || codVecindad == 32 || codVecindad == 64 || codVecindad == 128);
}


// Colorea un vector de Points en una Mat binaria con el valor "intensidad"
void limpia::marcarPuntos(vector <Point *> vec, Mat &mat, uchar intensidad) {
    Point *pun;
    for(vector<Point *>::iterator it = vec.begin(); it != vec.end(); ++it) {
        pun = *it;
        mat.at<uchar>(pun->y, pun->x) = intensidad;
    }
}

void limpia::marcarTrazo(vector <Point *> vec, Mat &img) {
    Point *pun;
    for(vector<Point *>::iterator it = vec.begin(); it != vec.end(); ++it) {
        pun = *it;
        img.at<uchar>(pun->y, pun->x) = 255;
    }
}

// Si en (x,y) hay un trazo, lo elimina
uchar limpia::trazoAislado(Mat &img, int x, int y) {
    uchar codVecindad = getCodVecindad(img, x, y);

    // Si es un punto final de línea...
    if (codPuntoFinal(codVecindad)) {
        // Marcamos el trazo mientras se conserve delgado (1px)
        bool continuar = true, marcar = true;
        uchar dir = 0;
        uchar codSig;
        int i, j;
        int xAnt = x, yAnt = y, xSig, ySig;
        int xMin = x, yMin = y, xMax = x, yMax = y;
        int numMarcas = 0, numNegros = 0;

        vector <Point *> vecTrazo;

        while (continuar) {
            vecTrazo.push_back(new Point(xAnt, yAnt));

            switch (codVecindad) {
                case 1: dir = 1; xSig=xAnt+1; ySig=yAnt-1; break;
                case 2: dir = 2; xSig=xAnt+1; ySig=yAnt; break;
                case 4: dir = 3; xSig=xAnt+1; ySig=yAnt+1; break;
                case 8: dir = 4; xSig=xAnt; ySig=yAnt+1; break;
                case 16: dir = 5; xSig=xAnt-1; ySig=yAnt+1; break;
                case 32: dir = 6; xSig=xAnt-1; ySig=yAnt; break;
                case 64: dir = 7; xSig=xAnt-1; ySig=yAnt-1; break;
                case 128: dir = 0; xSig=xAnt; ySig=yAnt-1; break;
                //default: cout << "(" << xAnt << "," << yAnt << ") codigo " << (int)codVecindad << endl; return 0; break;
            }

            // Actualizamos bounding box
            if (xSig < xMin) xMin = xSig;
            else if (xSig > xMax) xMax = xSig;
            if (ySig < yMin) yMin = ySig;
            else if (ySig > yMax) yMax = ySig;

            codSig = getCodVecindad(img, xSig, ySig);
            if (codPuntoFinal(codSig)) {
                // el trazo terminó
                vecTrazo.push_back(new Point(xSig, ySig));
                continuar = false;
            }
            else {
                // la línea no termina aún. Borrar pixel y seguir la línea
                img.at<uchar>(yAnt, xAnt) = 255;
                yAnt = ySig;
                xAnt = xSig;
                codVecindad = getCodVecindad(img, xSig, ySig);
                continuar = codPuntoFinal(codVecindad);
            }
        }

        // contamos el número de pixeles marcados y no marcados en la vecindad
        /*
        for (i = (xMin - 1); i <= (xMax + 1); i++) {
            if (img.at<uchar>(yMin - 1, i) == 255) {
                //(imgMarcas.at<uchar>(yMin - 1, i) == 255) ? numMarcas++ : numNegros++;
            }
            if (img.at<uchar>(yMax + 1, i) == 255) {
                //(imgMarcas.at<uchar>(yMax + 1, i) == 255) ? numMarcas++ : numNegros++;
            }
        }
        for (j = yMin; j <= yMax; j++) {
            if (img.at<uchar>(j, xMin - 1) == 255) {
                //(imgMarcas.at<uchar>(j, xMin - 1) == 255) ? numMarcas++ : numNegros++;
            }
            if (img.at<uchar>(j, xMax + 1) == 255) {
                //(imgMarcas.at<uchar>(j, xMax + 1) == 255) ? numMarcas++ : numNegros++;
            }
        }

        //cout << "(" << xMin << "," << yMin << ")-(" << xMax << "," << yMax << "): n" << numNegros << "/ m" << numMarcas << endl;

        if (numMarcas == 0 && numNegros == 0) { marcar = true; }
        else if (numMarcas > numNegros) { marcar = true; }
        */

        if (marcar) {
            marcarTrazo(vecTrazo, img);
        }

        return 1; // TODO: contar pixeles totales marcados
    }
    return 0;
}

int limpia::exec_limpiarTrazosAislados(Mat &img) {
    int imgAlto = img.rows;
    int imgAncho = img.cols;
    int x, y, numTrazosMarcados = 0;
    uchar* fila;
    uchar intensidad, a;

    for (y = 1 ; y < (imgAlto - 1) ; ++y) {
        fila = img.ptr<uchar>(y);
        for (x = 1 ; x < (imgAncho - 1) ; ++x) {
            intensidad = fila[x]; // valor de intensidad del pixel
            if (intensidad == 255) {
                a = trazoAislado(img, x, y);
                if (a > 0) { numTrazosMarcados++; }
            }
        }
    }

    cout << "metodoTrazoAislado marco " << numTrazosMarcados << " trazos." << endl;
    return numTrazosMarcados;
}


// true si el pixel se encuentra aislado
uchar limpia::pixelAislado(Mat &img, int x, int y) {
    uchar codVecindad = getCodVecindad(img, x, y); // aplicamos codificación Greenlee (1987)
    //cout << x << "," << y << " " << (int)codVecindad << endl;
    return (codVecindad == 0);
}

// true si se detecta un pixel blanco aislado
bool limpia::pixelNegroAislado(Mat &img, int x, int y) {
    uchar c = getCodVecindad(img, x, y);
    return (c == 255 || c == 127 || c == 253 || c == 247 || c == 223 || c == 254 || c == 251 || c == 239 || c == 191);
}

// Resibe una imagen en escala de grises
// Remueve los pixeles aislados
// Devuelve en imgDest
int limpia::exec_limpiarPxAislados(Mat &img) {
    int imgAlto = img.rows;
    int imgAncho = img.cols;
    int x, y, numPxMarcados = 0;
    uchar* fila;
    uchar intensidad;

    for (y = 1 ; y < (imgAlto - 1) ; ++y) {
        fila = img.ptr<uchar>(y);
        for (x = 1 ; x < (imgAncho - 1) ; ++x) {
            intensidad = fila[x];
            if (intensidad == 0) {
                if (pixelAislado(img, x, y)) {
                    img.at<uchar>(y, x) = 255;
                    numPxMarcados++;
                }
            }
            else {
                if (pixelNegroAislado(img, x, y)) {
                    img.at<uchar>(y, x) = 0;
                    numPxMarcados++;
                }
            }
        }
    }

    return numPxMarcados;
}


void limpia::exec_limpiarPxBordes(Mat &img) {
    unsigned short int imgAlto = img.rows - 1;
    unsigned short int imgAncho = img.cols - 1;
    unsigned short int x, y;
    uchar* fila;
    uchar intensidad, cod;

    for (y = 1 ; y < imgAlto ; ++y) {
        fila = img.ptr<uchar>(y);
        for (x = 1 ; x < imgAncho ; ++x) {
            intensidad = fila[x];
            if (intensidad == 0) {
                cod = getCodVecindad(img, x, y);
                switch (cod) {
                case 61: img.at<uchar>(y-1, x+1) = 255; break;
                case 244: img.at<uchar>(y+1, x+1) = 255; break;
                case 211: img.at<uchar>(y+1, x-1) = 255; break;
                case 79: img.at<uchar>(y-1, x-1) = 255; break;
                case 94: img.at<uchar>(y-1, x-1) = 255; break;
                case 121: img.at<uchar>(y-1, x+1) = 255; break;
                case 229: img.at<uchar>(y+1, x+1) = 255; break;
                case 151: img.at<uchar>(y+1, x-1) = 255; break;

                }


            }
        }
    }
}
