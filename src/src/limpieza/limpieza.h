/*
 * File:   limpieza.h
 * Author: Dany Alejandro
 *
 * Created on 18 de mayo de 2014, 04:02 PM
 */

#ifndef LIMPIEZA_H
#define	LIMPIEZA_H

#include "opencv2/core/core.hpp"

using namespace cv;

namespace limpia {

// Devuelve el código de la vecindad de un pixel
uchar getCodVecindad(Mat &img, int x, int y);
// true si es el código greenlee de un punto final de línea
bool codPuntoFinal(uchar codVecindad);
// Colorea de blanco un vector de Point en una Mat binaria
void marcarPuntos(vector <Point *> vec, Mat &mat, uchar intensidad);
void marcarTrazo(vector <Point *> vec, Mat &img);
uchar trazoAislado(Mat &img, int x, int y);
// true si el pixel se encuentra aislado
uchar pixelAislado(Mat &img, int x, int y);
bool pixelNegroAislado(Mat &img, int x, int y);

// Recibe una imagen en escala de grises
// Remueve los pixeles aislados
// Devuelve en imgDest
int exec_limpiarPxAislados(Mat &imgOrigi);
int exec_limpiarTrazosAislados(Mat &imgOrigi);

// Remueve los pixeles de borde
void exec_limpiarPxBordes(Mat &imgOrigi);
}

#endif	/* LIMPIEZA_H */

