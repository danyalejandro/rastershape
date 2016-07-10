#ifndef INFOMAPA_H
#define INFOMAPA_H

#include <QObject>
#include <QString>
#include <QDebug>
#include <QPoint>

#include <iostream>

// Clase que almacena la información del mapa, sus coordenadas y recorte
// Anchos y altos se auto-calculan cuando se ingresan las coordenadas con los setters
class InfoMapa: public QObject {
Q_OBJECT
private:

public:
    // Info de la imagen original
    unsigned short int imgAnchoPx, imgAltoPx;
    QString imgRuta;

    // Info del recorte (respecto a la imagen original)
    unsigned short int recAnchoPx, recAltoPx;
    unsigned short int recXmin, recYmin, recXmax, recYmax;

    // Info de los 4 puntos de referencia (pixeles y coords. planas)
    unsigned short int aX, aY, bX, bY, cX, cY, dX, dY;
    double aN, aE, bN, bE, cN, cE, dN, dE;

    bool bordeAgregado; // TRUE si se ha agregado un borde de 1px a la imagen

    explicit InfoMapa(QObject *parent = 0);
    Q_INVOKABLE void setInfoImg(int ancho, int alto, QString ruta);
    Q_INVOKABLE void setInfoRecorte(int x1, int y1, int x2, int y2);
    Q_INVOKABLE void setPosPuntosRef(int ax, int ay, int bx, int by, int cx, int cy, int dx, int dy);
    Q_INVOKABLE void setCoordsPuntosRef(int ae, int an, int be, int bn, int ce, int cn, int de, int dn);

    Q_INVOKABLE QPoint getCoords(int x, int y);

    Q_INVOKABLE void imprimir();

    void initCoordsDefault();
};

#endif // INFOMAPA_H
