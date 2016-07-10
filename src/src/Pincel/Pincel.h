#ifndef PINCEL_H
#define	PINCEL_H

#include <math.h>
#include <iostream>
#include <deque>

#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

#include "Constantes.h"
#include "PuntoAnalizado.h"
#include "Recta.h"
#include "utileria.h"

using namespace cv;
using namespace std;

struct InfoRecorrido {
    Point puntoIni; // Punto de inicio del recorrido
    Point puntoFin; // Punto final del recorrido
    float anguloIni; // Ángulo inicial desde el punto de inicio
};

class Pincel {
public:
    float radio; // Radio del pincel
    short int periodoControl; // Número de puntos recorridos por el pincel para tomar una muestra de ángulo
    short int puntosAngEstable; // Número de puntos de control donde se considera que ya se obtuvo la dirección de propagación
    short int numPixEncolar; // Número de vecinos a encolar  (1..8)
    float cambioAngMax; // máximo cambio en el ángulo permitido para analizar un punto
    float maxDistGuia; // distancia máxima que se permite a los puntos alejarse de la guía para ser analizados
    float guiaLongMin; // longitud mínima de la recta guía empleada para corregir el ángulo
    InfoRecorrido infoRecIni; // información del primer recorrido realizado

    vector <Point> puntosTrazoPin; // Puntos del trazo del pincel
    Mat imgPincelTrazo; // Referencia a Mat en la que se marcan los puntos procesados de este trazo
    Mat imgPincel; // Referencia a la Mat con todos los trazos

    VideoWriter debugVideo;

    Pincel(Mat &img);
    bool recorrerDoble(Point pIni);
    bool recorrerDireccion(Point pIni, bool calcAngIni, bool calcPuntoIni);
    bool recorrerInverso();
    void guardarDebug();
    void mostrarDebug(bool ampliar);
    void initAnguloRecorrido(float angulo);

private:
    Mat imgBase; // Referencia a la imagen binaria de base
    Mat imgRecorrido; // Marcas de pixeles ya visitados
    Mat imgDebug; // Imagen RGB auxiliar de marcas de depuración

    Vector<float> dirsInicio; // Direcciones iniciales posibles desde el punto inicial

    Recta rectaGuia; // Parámetros de la ecuación de la recta guía
    CuerpoPix cuerpoIni; // Cuerpo usado para calcular el punto de inicio del recorrido

    Point2f puntoPincel; // coordenadas del centro del pincel (imaginarias)
    Point2f puntoUltControl; // último punto de muestra tomada

    Point ultPuntoRecorrido; // último punto del recorrido
    float ultAnguloRecorrido; // Último ángulo del recorrido ya terminado
    int numRecorrido; // Número de recorrido actual

    unsigned int numPixel; // Cuenta de pixeles recorridos

    float anguloControl[5]; // últimas muestras de ángulo tomadas (la posición 0 es la más reciente)
    float anguloEstim; // ángulo siguiente estimado
    float anguloCambioProm; // cambio promedio ponderado calculado de los ángulos anteriores

    int cuentaInternaControl; // cuenta interna para tomar nueva muestra de ángulo
    int numControl; // muestras de ángulo tomadas

    uchar ordenesEnc[8][8]; // arreglos de orden de encolado por dirección (posiciones: [0:arr, 1:arrDer, 2:der, 3:abaDer, 4:aba, 5:abaIzq, 6:izq, 7:arrIzq])
    uchar *ordenEncolado; // Orden de encolado seleccionado
    deque <Point> pendientes; // Puntos pendientes por analizar
    deque<PuntoAnalizado> puntosAnaliz; // Últimos puntos analizados

    // Recorrido de trazo básico
    void analizarPunto(Point punto);
    void encolarVecinos(Point punto);
    void moverPincel(Point punto);
    void puntoControl();
    void estimarSigAngulo();
    void actOrdenEncolado();
    void encolarPunto(Point punto);
    void _agrOrdenEncolado(uchar index, uchar n0, uchar n1, uchar n2, uchar n3, uchar n4, uchar n5, uchar n6, uchar n7);
    void guardarPunAna(PuntoAnalizado punAna);

    // *** TECNICAS

    // Recta guía
    bool validarDistGuia(PuntoAnalizado &punAna);
    void corregirDirProp(uchar intentos, float incremento);
    void setRectaGuia(float x0, float y0, float xMax, float yMax, float l);

    // Corrección de punto inicial
    bool centrarEnLinea(Point &punto);
    bool inicializarDireccion();
    float anguloCuerpoPix(CuerpoPix cuerpo);
    void angulosProyeccion(Point punto);

    // Salto de discontinuidades
    bool buscarDiscontinuidad(Point &sigPunto);

    // *** MARCADO Y DEPURACIÓN

    // Marcado
    void marcaRecorrido(Point punto);
    void desmarcaPuntoRecorrido(Point punto);
    void marcaTrazoPincel(Point punto);

    // Ventanas
    void mostrarRecorrido(bool ampliar);

    // Depuración
    void initMatDebug();
    void marcaDebug(Point punto, uchar nColor);
};
#endif	/* PINCEL_H */
