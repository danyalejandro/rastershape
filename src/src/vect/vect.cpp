#include "vect.h"
#include "transform.h"

// Obtiene la vecindad de un punto de la imagen binaria (1 solo si es un pixel ocupado)
// Obligatorio recibir por parámetro uchar vecindad[8]; ya que no se puede crear y devolver arreglos
void *vect::vecindad(Mat &img, Point pun, uchar vec[]) {
    vec[0] = (img.at<uchar>(pun.y-1, pun.x) == 0) ? 1 : 0;
    vec[1] = (img.at<uchar>(pun.y-1, pun.x+1) == 0) ? 1 : 0;
    vec[2] = (img.at<uchar>(pun.y, pun.x+1) == 0) ? 1 : 0;
    vec[3] = (img.at<uchar>(pun.y+1, pun.x+1) == 0) ? 1 : 0;
    vec[4] = (img.at<uchar>(pun.y+1, pun.x) == 0) ? 1 : 0;
    vec[5] = (img.at<uchar>(pun.y+1, pun.x-1) == 0) ? 1 : 0;
    vec[6] = (img.at<uchar>(pun.y, pun.x-1) == 0) ? 1 : 0;
    vec[7] = (img.at<uchar>(pun.y-1, pun.x-1) == 0) ? 1 : 0;
}

// Devuelve el código Greenlee de la vecindad de un pixel
uchar vect::codVecindad(Mat &img, Point pun) {
    uchar vecinos[8];
    vecindad(img, pun, vecinos);
    return 128 * vecinos[0] + vecinos[1] + 2 * vecinos[2]
        + 4 * vecinos[3] + 8 * vecinos[4] + 16 * vecinos[5]
        + 32 * vecinos[6] + 64 * vecinos[7];
}

// TRUE si es punto final
bool vect::esPunFinal(Mat &img, Point pun) {
    uchar cod = codVecindad(img, pun);
    return (cod == 128 || cod == 2 || cod == 8 || cod == 32 || cod == 1 || cod == 4 || cod == 16 || cod == 64);
}

void vect::puntosFinales(Mat &img, vector<Point> &punFinales) {
    unsigned int x, y, numFilas, numCols;
    uchar* fila;
    uchar intensidad;
    Point pun;

    numFilas = img.rows;
    numCols = img.cols;

    // Recorremos la imagen pixel por pixel (excepto el borde)
    for (y = 1 ; y < (numFilas - 1) ; ++y) {
        fila = img.ptr<uchar>(y);
        for (x = 1 ; x < (numCols - 1) ; ++x) {
            intensidad = fila[x];
            if (intensidad == 0) {
                pun = Point(x, y);
                if (esPunFinal(img, pun)) {
                    punFinales.push_back(pun);
                }
            }
        }
    }
}

// Desplaza un punto en la dirección según el código greenlee especificado
void vect::moverPuntoCod(Point &pun, uchar cod) {
    switch (cod) {
        case 1: pun.x++; pun.y--; break;
        case 2: pun.x++; break;
        case 4: pun.x++; pun.y++; break;
        case 8: pun.y++; break;
        case 16: pun.x--; pun.y++; break;
        case 32: pun.x--; break;
        case 64: pun.x--; pun.y--; break;
        case 128: pun.y--; break;
    }
}

SHPObject *vect::toArc(vector<Point2f> vecPuntosTrazo, int imgHeight) {
    int numPuntos = vecPuntosTrazo.size();
    double coordsX[numPuntos], coordsY[numPuntos];

    int i;
    for (i = 0; i < numPuntos; i++) {
        coordsX[i] = vecPuntosTrazo[i].x ;
        coordsY[i] = vecPuntosTrazo[i].y ;
    }

    SHPObject *objShp = SHPCreateSimpleObject(SHPT_ARC, numPuntos, coordsX, coordsY, NULL);
    return objShp;
}

// A partir del trazo, genera y guarda el vector correspondiente en formato SHAPE
void vect::agregarShape(vector<Point2f> trazo, int altura, SHPHandle sh, transformGeo *objGeo, Point pA, DBFHandle dbf) {
    vector<Point2f> l;
    for(vector<Point2f>::iterator it = trazo.begin(); it != trazo.end(); ++it)
    {
        Point2f p = *it;
        double xi = pA.x+p.x-1.0;
        double yi = pA.y+p.y-1.0;
        double xp= objGeo->getX(xi, yi);
        double yp = objGeo->getY(xi, yi);
        Point2f pi = Point2f(xp,yp);
        l.push_back(pi);
    }
    SHPObject *objShp = toArc(l, altura);
    int polyid = SHPWriteObject(sh, -1, objShp);
    DBFWriteDoubleAttribute(dbf, polyid, 0, 0.0);
    cout<<polyid<<".,,"<<endl;
    SHPDestroyObject(objShp);
}


// mueve un punto de posición en una dirección (0:arr, 1:arrder, etc)
void vect::moverPunto(Point &punto, uchar dir) {
    switch (dir) {
        case 0: punto.y--; break;
        case 1: punto.x++; punto.y--; break;
        case 2: punto.x++; break;
        case 3: punto.x++; punto.y++; break;
        case 4: punto.y++; break;
        case 5: punto.x--; punto.y++; break;
        case 6: punto.x--; break;
        case 7: punto.x--; punto.y--; break;
    }
}


// Recorre un trazo desde pun y almacena sus puntos en trazo
// No removemos puntos colineales para mejorar los resultados del Ramer-Douglas-Peuker
void vect::recorrerTrazo(Mat &imgBase, Point pun, vector<Point2f> &trazo, bool trazoCerrado) {
    uchar cod;
    Point2f punIni;
    trazo.clear();

    // Validamos el trazo a recorrer
    if (imgBase.at<uchar>(pun) == 255) return;
    if (codVecindad(imgBase, pun) == 0) return;

    punIni = pun;
    trazo.push_back(punIni);

    // Si es trazo cerrado, borramos 1 pixel e iniciamos desde el siguiente pixel detectado
    if (trazoCerrado) {
        uchar i = 0;
        uchar vecinos[8];


        vecindad(imgBase, pun, vecinos);
        imgBase.at<uchar>(pun) = 255;
        while (vecinos[i] != 1) i++;
        moverPunto(pun, i);
    }


    while (imgBase.at<uchar>(pun) != 255) {
        cod = codVecindad(imgBase, pun);
        imgBase.at<uchar>(pun) = 255; // punto recorrido se borra

        if (cod != 0) {
            trazo.push_back(pun);
            moverPuntoCod(pun, cod);
        }
        else {
            if (trazo.size() > 0) trazo.push_back(pun);
        }
    }

    // Si es trazo cerrado, unir los últimos 2 puntos
    if (trazoCerrado) trazo.push_back(punIni);
}

/**
 * Vectoriza todos los trazos de una imagen binaria. Los trazos deben estar completamente adelgazados
 * @param imgBase Imagen binaria CV_8U
 * @param filename Nombre del archivo de salida (sin extensión)
 * @param deltaRDP Delta de reducción de puntos Ramen-Douglas-Peucker, entre más grande más agresivo, en 0 no se hace reducción de puntos (valores ejemplo: 0.5, 1, 2...)
 * @param suavizado Nivel de suavizado con interpolación spline Catmull-Rom, en 0 no se suavizan los trazos (valores ejemplo: 0, 1, 2, 4...)
 */
void vect::execVectorize(Mat &imgBase, const char *filename, float deltaRDP, short int suavizado, InfoMapa *iMapa) {
    SHPHandle sh = SHPCreate(filename, SHPT_ARC);
    DBFHandle dbf = DBFCreate(filename);
    int res = DBFAddField(dbf,"T_ALT", FTDouble, 12, 4);
    DBFClose(dbf);
    DBFHandle hDBF = DBFOpen(filename,"rb+");

    cout<<res<<",,,,,,,,,,,,,,,,,,,,,,,"<<endl;

    int imgAncho = imgBase.cols;
    int imgAlto = imgBase.rows;

    Point pA = Point(iMapa->recXmin,iMapa->recYmin);
    cout<<"XMIN"<<iMapa->recXmin<<endl;
    cout<<"YMIN"<<iMapa->recYmin<<endl;
/*
    Point pA = Point(492, 372);
    iMapa->setCoordsPuntosRef(1060000, 770000, 1075000, 770000, 1075000, 760000, 1060000, 760000);
    iMapa->setPosPuntosRef(522, 371,9957, 475, 9882, 6799, 453, 6701);
*/
    transformGeo *objGeo = new transformGeo();
    objGeo->setCoeficients(iMapa);
    // recorremos toda la imagen buscando puntos finales

    int altop = 0;
    vector<Point> punFinales;
    vect::puntosFinales(imgBase, punFinales);

    // para cada punto final, recorremos el trazo correspondiente y guardamos el vector
    Point pun;
    vector<Point2f> trazo;
    vector<Point2f> trazoSuave;
    for(vector<Point>::iterator it = punFinales.begin(); it != punFinales.end(); ++it) {
        pun = *it;
        cout << "recorrerTrazo() " << pun << endl;
        recorrerTrazo(imgBase, pun, trazo, false);

        if (trazo.size() > 1) {
            vec_RDP::reducirPuntos(trazo, deltaRDP); // aplicamos reducción de puntos Ramer-Douglas-Peucker
            catmull::interpolar(trazo, trazoSuave, suavizado);
            agregarShape(((suavizado > 0) ? trazoSuave : trazo), altop, sh, objGeo, pA, hDBF);
        }
    }

    // Buscamos en toda la imagen, si aún hay pixeles sin borrar, son de trayectos cerrados
    unsigned int x, y;
    uchar* fila;
    uchar intensidad;

    for (y = 1 ; y < (imgAlto - 1) ; ++y) {
        fila = imgBase.ptr<uchar>(y);
        for (x = 1 ; x < (imgAncho - 1) ; ++x) {
            intensidad = fila[x];
            if (intensidad == 0) {
                pun = Point(x, y);

                recorrerTrazo(imgBase, pun, trazo, true);

                if (trazo.size() > 1) {
                    vec_RDP::reducirPuntos(trazo, deltaRDP); // aplicamos reducción de puntos Ramer-Douglas-Peucker
                    catmull::interpolar(trazo, trazoSuave, suavizado);
                    agregarShape(((suavizado > 0) ? trazoSuave : trazo), imgAlto, sh, objGeo, pA, hDBF);
                }
            }
        }
    }
DBFClose(hDBF);
    SHPClose(sh);

}



//PARA CALCULO DE COORDENADAS
/*
double n = 4;
double sx = 11144.546;
double sy = 6398.103;
double sx2 = 52566553.35;
double sxy = 17864122.18;
double sy2 = 15426520.17;

double SX = 2094256.916;
double SY = 21014457.0;
double SxX = 5878608371.0;
double SxY = 58549300974;
double SyX = 3349873336;
double SyY = 33623717018;

int altop = 770000;
*/
/*
pA.x = 0.0;
pA.y = 0.0;

pB.x = 10423.0;
pB.y = 8146.0;

Point2f pa = Point(0.0,0.0);
Point2f pb = Point(0.0,0.0);
Point2f pc = Point(0.0,0.0);
Point2f pd = Point(0.0,0.0);

pa.x = 518.950351;
pa.y = 447.838268;

pb.x = 9956.705532;
pb.y = 449.388431;

pc.x = 9953.940401;
pc.y = 6737.896063;

pd.x = 521.548975;
pd.y = 6740.193480;

int altop = 770000;

aLat = 770000.0;
aLon = 1045000.0;

bLat = 770000.0;
bLon = 1060000.0;

cLat = 760000.0;
cLon = 1060000.0;

dLat = 760000.0;
dLon = 1045000.0;
*//*
double n = 4;
double sx =  pa.x + pb.x + pc.x + pd.x;
//float js = pa.x + pb.x + pc.x + pd.x;
double sy = pa.y + pb.y + pc.y + pd.y;
double sx2 = (pa.x*pa.x) + (pb.x*pb.x) + (pc.x*pc.x) + (pd.x*pd.x);
double sxy = (pa.x*pa.y)+(pb.x*pb.y)+(pc.x*pc.y)+(pd.x*pd.y);
double sy2 = (pa.y*pa.y) + (pb.y*pb.y) + (pc.y*pc.y) + (pd.y*pd.y);

double SX = aLon+bLon+cLon+dLon;
double SY = aLat+bLat+cLat+dLat;
double SxX = (aLon*pa.x)+(bLon*pb.x)+(cLon*pc.x)+(dLon*pd.x);
double SxY = (aLat*pa.x)+(bLat*pb.x)+(cLat*pc.x)+(dLat*pd.x);
double SyX = (aLon*pa.y)+(bLon*pb.y)+(cLon*pc.y)+(dLon*pd.y);
double SyY = (aLat*pa.y)+(bLat*pb.y)+(cLat*pc.y)+(dLat*pd.y);
*/

/*
    cout<<sx<<"sx"<<endl;
    cout<<sy<<"sy"<<endl;
    cout<<sx2<<"sx2"<<endl;
    cout<<sy2<<"sy2"<<endl;
    cout<<sxy<<"sxy"<<endl;
    cout<<"-------------------------------------"<<endl;
    cout<<SX<<"SX"<<endl;
    cout<<SY<<"SY"<<endl;
    cout<<SxX<<"SxX"<<endl;
    cout<<SxY<<"SxY"<<endl;
    cout<<SyX<<"SyX"<<endl;
    cout<<SyY<<"SyY"<<endl;
*/
