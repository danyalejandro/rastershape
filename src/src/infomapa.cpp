#include "infomapa.h"
#include "vect/transform.h"
InfoMapa::InfoMapa(QObject *parent) : QObject(parent) {
    setInfoImg(0, 0, "");
    setInfoRecorte(0, 0, 0, 0);
    setPosPuntosRef(0, 0, 0, 0, 0, 0, 0, 0);
    setCoordsPuntosRef(0, 0, 0, 0, 0, 0, 0, 0);
    bordeAgregado = false;
}

void InfoMapa::setInfoImg(int ancho, int alto, QString ruta) {
    imgAnchoPx = ancho;
    imgAltoPx = alto;
    imgRuta = ruta;
}

// Retorna las coordenadas este y norte de un pixel ubicado en (x, y)
QPoint InfoMapa::getCoords(int x, int y) {
    transformGeo *objGeo = new transformGeo();
   objGeo->setCoeficients(this);
   //Point pA = Point(this->recXmin,this->recYmin);
   double xi = this->recXmin + x;
   double yi = this->recYmin + y;
   double xp = objGeo->getX(xi, yi);
   double yp = objGeo->getY(xi, yi);
   return QPoint(xp, yp);
}


/**
 * @brief InfoMapa::setInfoRecorte Establece la posición en pixeles del recorte.
 * Los valores mínimos y máximos son inclusivos.
 */
void InfoMapa::setInfoRecorte(int x1, int y1, int x2, int y2) {
    recXmin = x1;
    recYmin = y1;
    recXmax = x2;
    recYmax = y2;

    if (x2 - x1 == 0) {
        recAnchoPx = 0;
        recAltoPx = 0;
    } else {
        recAnchoPx = x2 - x1 + 1;
        recAltoPx = y2 - y1 + 1;
    }
}

/**
 * @brief InfoMapa::setPosPuntosRef Setter de la posición en pixeles de los 4 puntos de referencia
 */
void InfoMapa::setPosPuntosRef(int ax, int ay, int bx, int by, int cx, int cy, int dx, int dy) {
    aX = ax; aY = ay;
    bX = bx; bY = by;
    cX = cx; cY = cy;
    dX = dx; dY = dy;
}

/**
 * @brief InfoMapa::setCoordsPuntosRef Setter de la posición en coords. planas de los 4 puntos de referencia
 */
void InfoMapa::setCoordsPuntosRef(int ae, int an, int be, int bn, int ce, int cn, int de, int dn) {
    aE = ae; aN = an;
    bE = be; bN = bn;
    cE = ce; cN = cn;
    dE = de; dN = dn;
}

// Imprime todos los datos almacenados en este objeto
void InfoMapa::imprimir() {
    qDebug() << "Imagen: " << imgRuta << " (" << imgAnchoPx << " x " << imgAltoPx << ")";
    qDebug() << "Recorte: " << "de (" << recXmin << ", " << recYmin << ") a (" << recXmax << ", " << recYmax << ") dimensiones: " << recAnchoPx << " x " << recAltoPx;
    qDebug() << "Pos. Marcas: " << "(" << aX << ", " << aY << "), " << "(" << bX << ", " << bY << "), " << "(" << cX << ", " << cY << "), " << "(" << dX << ", " << dY << ")";
    qDebug() << "Coord. Marcas: " << "(" << aE << ", " << aN << "), " << "(" << bE << ", " << bN << "), " << "(" << cE << ", " << cN << "), " << "(" << dE << ", " << dN << ")";
    qDebug() << "Borde agregado:" << bordeAgregado;
}

void InfoMapa::initCoordsDefault() {
    setPosPuntosRef(0, 0, imgAnchoPx - 1, 0, imgAnchoPx - 1, imgAltoPx - 1, 0, imgAltoPx - 1);
    setCoordsPuntosRef(0, imgAltoPx - 1, imgAnchoPx - 1, imgAltoPx - 1, imgAnchoPx - 1, 0, 0, 0);
    setInfoRecorte(0, 0, imgAnchoPx - 1, imgAltoPx - 1);
    imprimir();
}
