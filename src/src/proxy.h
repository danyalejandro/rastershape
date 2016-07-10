#ifndef PROXY_H
#define PROXY_H

#include <QObject>
#include <QString>

#include <iostream>
#include <math.h>
#include <time.h>

#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

#include "limpieza/limpieza.h"
#include "Pincel/utileria.h"
#include "Pincel/Pincel.h"

#include "thinning/thinning.h"
#include "reconstruction/lineReconstruction.h"

#include "vect/shapelib/shapefil.h"
#include "vect/vect.h"
#include "vect/vect_RDP.h"

#include "opencvimgprovider.h"
#include "infomapa.h"



using namespace cv;
using namespace std;

// Coordenadas geográficas y raster de un punto
struct puntoCoords {
    uchar num; // número de marca [0..3]
    bool cuadranteIzq, cuadranteArr; // Cuadrante de esta marca respecto a las otras marcas
    unsigned int x, y, este, norte;

};

class Proxy : public QObject {
    Q_OBJECT
private:
    Mat imgBinaria; // Sobre la que se trabaja
    Mat imgTrazo; // Almacena el trazo actual
    Mat imgPincel; // Acumula el resultado de marcar trazos

    Pincel *pin;
    Point puntoIniPincel; // Posición inicial de último recorrido de pincel
    unsigned short int cuentaPuntosPincel; // Contador para iterar en el vector de puntos de trazo

    uchar ordenMarcas[4]; // Orden de las marcas
    puntoCoords iMarcas[4];

    void marcarPuntosBW(Mat &origen, Mat &destino);
    void marcarPuntosCol(Mat &origen, Mat &destino, Vec3b color);
    Rect MBR(Mat &img);

public:
    OpencvImgProvider *provider;
    InfoMapa *iMapa;

    explicit Proxy(QObject *parent = 0);

    Q_INVOKABLE QString getCoordsUri(int cuadrante);

    Q_INVOKABLE bool cargarImagen(QString ruta);
    bool agregarBorde();
    void dibujarCruz(Mat &img, int x, int y, uchar color);

    Q_INVOKABLE void limpiarAislados();
    Q_INVOKABLE void limpiarBordes();

    Q_INVOKABLE void initEtapaPincel();
    Q_INVOKABLE void marcarTrazo(int x, int y, float radio, uchar numVecinos, float cambioAngMax);
    Q_INVOKABLE void reintentarRecorrido();
    Q_INVOKABLE void guardarTrazo();
    Q_INVOKABLE void aislarTrazos();
    Q_INVOKABLE void borrarTrazos();
    Q_INVOKABLE void pintar(int x, int y, bool rellenar, int tamano);

    Q_INVOKABLE void setPosMarca(int numMarca, int x, int y);
    Q_INVOKABLE void setCoordsMarca(int numMarca, int este, int norte);
    void setInfoMarcas();
    Q_INVOKABLE void recortar(int x, int y, int ancho, int alto);

    Q_INVOKABLE void adelgazar(bool desenfocar);
    Q_INVOKABLE void reconstruir(int h);
    Q_INVOKABLE void vectorizar(float redPuntos, unsigned int suavizado, QString ruta);

    Q_INVOKABLE void guardarImagen(QString ruta);

signals:

public slots:

};

#endif // PROXY_H
