#include <deque>

#include "utileria.h"

/**
 * Obtiene el cuerpo de pixeles detectado desde pun, con un número máximo de pixeles y lo almacena en cuerpo
 * @param p
 * @param cuerpo
 */
void getCuerpoPix(Point punIni, CuerpoPix &cuerpo, Mat &img, Mat &imgMarcas, int numPixMax, uchar numOrden, uchar numVecinosCola) {
    Point punto = punIni;
    Point punVecino = punIni;
    deque <Point> puntos;
    uchar vecinos[8], vecinosReco[8], dir;
    int cuentaPixeles = 0;
    int i, x1 = punto.x, y1 = punto.y, x2 = punto.x, y2 = punto.y;

    cuerpo.pIni = punto;
    puntos.push_back(punto);
    imgMarcas.at<uchar>(punto) = 0;

    // Valores por defecto, necesarios por si se parte de uno de los extremos
    cuerpo.pIzq = punto;
    cuerpo.pDer = punto;
    cuerpo.pArr = punto;
    cuerpo.pAba = punto;

    uchar ord[8];
    switch (numOrden) {
        case 0: ord[0] = 0; ord[1] = 1; ord[2] = 7; ord[3] = 2; ord[4] = 6; ord[5] = 3; ord[6] = 5; ord[7] = 4; break;
        case 1: ord[0] = 1; ord[1] = 2; ord[2] = 0; ord[3] = 3; ord[4] = 7; ord[5] = 4; ord[6] = 6; ord[7] = 5; break;
        case 2: ord[0] = 2; ord[1] = 3; ord[2] = 1; ord[3] = 4; ord[4] = 0; ord[5] = 5; ord[6] = 7; ord[7] = 6; break;
        case 3: ord[0] = 3; ord[1] = 4; ord[2] = 2; ord[3] = 5; ord[4] = 1; ord[5] = 6; ord[6] = 0; ord[7] = 7; break;
        case 4: ord[0] = 4; ord[1] = 5; ord[2] = 3; ord[3] = 6; ord[4] = 2; ord[5] = 7; ord[6] = 1; ord[7] = 0; break;
        case 5: ord[0] = 5; ord[1] = 6; ord[2] = 4; ord[3] = 7; ord[4] = 3; ord[5] = 0; ord[6] = 2; ord[7] = 1; break;
        case 6: ord[0] = 6; ord[1] = 7; ord[2] = 5; ord[3] = 0; ord[4] = 4; ord[5] = 1; ord[6] = 3; ord[7] = 2; break;
        case 7: ord[0] = 7; ord[1] = 0; ord[2] = 6; ord[3] = 1; ord[4] = 5; ord[5] = 2; ord[6] = 4; ord[7] = 3; break;
        default: ord[0] = 0; ord[1] = 1; ord[2] = 2; ord[3] = 3; ord[4] = 4; ord[5] = 5; ord[6] = 6; ord[7] = 7; break;
    }

    while (cuentaPixeles < numPixMax && puntos.size() > 0) {
        punto = puntos.front();
        puntos.pop_front();
        vecindad(img, punto, vecinos);
        vecindad(imgMarcas, punto, vecinosReco);
        //cout << "(getCuerpoPix) Analizando " << punto << endl;
        cuentaPixeles++;

        for (i = 0 ; i < numVecinosCola ; i++) {
            dir = ord[i];
            if (vecinos[dir] == 1 && vecinosReco[dir] == 0) {
                punVecino = moverPosicion(punto, dir);
                imgMarcas.at<uchar>(punVecino) = 0;
                puntos.push_back(punVecino);
                //cout << "(getCuerpoPix) Encolando " << punVecino << endl;
            }
        }


        if (punto.x < x1) { x1 = punto.x; cuerpo.pIzq = punto; }
        else if (punto.x > x2) { x2 = punto.x; cuerpo.pDer = punto; }
        if (punto.y < y1) { y1 = punto.y; cuerpo.pArr = punto; }
        else if (punto.y > y2) { y2 = punto.y; cuerpo.pAba = punto; }
    }

    cuerpo.mbr = Rect(x1, y1, x2 - x1 + 1, y2 - y1 + 1);
    cuerpo.pFin = punto;
    rectangle(imgMarcas, Point(x1 - 1, y1 - 1), Point(x2 + 1, y2 + 1), Scalar(255), CV_FILLED); // eliminamos las marcas temporales

    cout << "(getCuerpoPix) MBR: " << cuerpo.mbr << ", izq: " << cuerpo.pIzq << ", der: " << cuerpo.pDer << ", arr: " << cuerpo.pArr << ", aba: " << cuerpo.pAba << endl;
}


/**
 * Dibuja una flecha
 * Forma de uso: flecha(imgOrigi, 100, 100, 10, 0, Scalar(255, 0, 0));
 * @param img Imagen sobre la cual dibujar.
 * @param xi Coord x de salida.
 * @param yi Coord y de salida.
 * @param mag Longitud del segmento en pixeles.
 * @param angulo Angulo en radianes.
 * @param color Color como un Scalar(B, G, R).
 */
void flecha(Mat &img, float xi, float yi, float mag, float angulo, Scalar color) {
    // obtenemos el punto final de la flecha
    float xf = xi + mag * cos(angulo);
    float yf = yi + mag * sin(angulo);

    // dibujamos la línea
    line(img, Point2f(xi, yi), Point2f(xf, yf), color, 2);

    // punto final de las líneas de la cabeza
    short int magF = 6;
    float ang1 = angulo + M_PI + (M_PI * 0.3);
    float ang2 = angulo - M_PI - (M_PI * 0.3);
    float cab1x = xf + magF * cos(ang1);
    float cab1y = yf + magF * sin(ang1);
    float cab2x = xf + magF * cos(ang2);
    float cab2y = yf + magF * sin(ang2);
    //cout << ang1*180/M_PI << "° (" << cab1x << ", " << cab1y << ")" << endl;

    line(img, Point2f(cab1x, cab1y), Point2f(xf, yf), color, 2);
    line(img, Point2f(cab2x, cab2y), Point2f(xf, yf), color, 2);
}


// Convierte radianes a grados
float rad2deg(float angulo) {
    return angulo * 180 / 3.1415926;
}


/**
 * Convierte un ángulo en radianes a una dirección entre 0 y 7
 * Encontramos la dirección aproximada [0:der, 1:dersup, 2:sup, 3:supizq, 4:izq, 5:izqInf, 6:inf, 7:infDer]
 * @param ang ángulo en radianes
 * @return dirección correspondiente
 */
uchar rad2dir(float ang) {
    int dir = 2;

    if (ang < 0) ang = anguloPositivo(ang);

    if (ang >= M_PI_8 && ang < 3 * M_PI_8) dir = 3;
    else if (ang >= 3 * M_PI_8 && ang < 5 * M_PI_8) dir = 4;
    else if (ang >= 5 * M_PI_8 && ang < 7 * M_PI_8) dir = 5;
    else if (ang >= 7 * M_PI_8 && ang < 9 * M_PI_8) dir = 6;
    else if (ang >= 9 * M_PI_8 && ang < 11 * M_PI_8) dir = 7;
    else if (ang >= 11 * M_PI_8 && ang < 13 * M_PI_8) dir = 0;
    else if (ang >= 13 * M_PI_8 && ang < 15 * M_PI_8) dir = 1;
    return dir;
}

// Retorna la dirección opuesta
uchar dirOpuesta(uchar dir) {
    switch (dir) {
        case 0: return 4; break;
        case 1: return 5; break;
        case 2: return 6; break;
        case 3: return 7; break;
        case 4: return 0; break;
        case 5: return 1; break;
        case 6: return 2; break;
        case 7: return 3; break;
    }

    return 0;
}

// Devuelve el ángulo expresado como un número positivo
float anguloPositivo(float a) {
    if (a < 0) return (2 * M_PI + a);
    return a;
}

// Devuelve el ángulo opuesto normalizado
float anguloOpuesto(float a) {
    a = normalizar(a);
    if (a > 0) return -(M_PI - a);
    else return (M_PI + a);
}

// Devuelve el ángulo en su menor expresión posible (positivo o negativo, de 0 a 180 grados)
float normalizar(float a) {
    if (a > M_PI) return - (2 * M_PI - a);
    else if (a < -M_PI) return (2 * M_PI + a);
    return a;
}


/**
 * Devuelve el ángulo NORMALIZADO de un punto medido desde el eje +x, teniendo en cuenta que según el cuadrante el resultado cambia (ej: -1, 1)
 * Este ángulo no se puede obtener de un atan simple
 * @param p Punto del cual se obtendrá el ángulo
 * @return ángulo correspondiente
 */
float anguloPunto(Point2f p) {
    float angTriang = atan(abs(p.y / p.x));
    float anguloNorm;

    // segun el cuadrante, modificar el ángulo para obtener uno relativo al origen
    //cout << "(anguloPunto) dy=" << p.y << ", dx=" << p.x << ", atan=" << rad2deg(angTriang) << "°";
    if (p.x >= 0 && p.y >= 0) {
        anguloNorm = angTriang;
    } else if (p.x <= 0 && p.y >= 0) {
        anguloNorm = M_PI - angTriang;
    } else if (p.x <= 0 && p.y <= 0) {
        anguloNorm = M_PI + angTriang;
    } else {
        anguloNorm = 2 * M_PI - angTriang;
    }
    //cout << "; angNorm: " << rad2deg(anguloNorm) << "°" << endl;

    return anguloNorm;
}


// mueve un punto de posición en una dirección (0:arr, 1:arrder, etc)
Point moverPosicion(Point punto, uchar dir) {
    //cout << "moverPosicion " << punto << "en dir: " << (int)dir << endl;
    Point nuevoPun = punto;
    switch (dir) {
        case 0: nuevoPun.y--; break;
        case 1: nuevoPun.x++; nuevoPun.y--; break;
        case 2: nuevoPun.x++; break;
        case 3: nuevoPun.x++; nuevoPun.y++; break;
        case 4: nuevoPun.y++; break;
        case 5: nuevoPun.x--; nuevoPun.y++; break;
        case 6: nuevoPun.x--; break;
        case 7: nuevoPun.x--; nuevoPun.y--; break;
    }

    return nuevoPun;
}


// Obtiene y retorna en el parámetro el vector unitario correspondiente al ángulo
void angulo2vector(float a, Point2f &p) {
    p.x = cos(a);
    p.y = sin(a);
}


// Devuelve el menor ángulo POSITIVO entre 2 puntos
float anguloDifPuntos(Point2f p1, Point2f p2) {
    float anguloNorm;
    float dx = p2.x - p1.x;
    float dy = p2.y - p1.y;

    if (dx == 0 && dy == 0) return 0;

    float angTriang = atan(abs(dy / dx));

    // segun el cuadrante, modificar el ángulo para obtener uno relativo al origen
    //cout << "(anguloPuntos) dy=" << dy << ", dx=" << dx << ", atan=" << rad2deg(angTriang) << "°";
    if (dx >= 0 && dy >= 0) {
        anguloNorm = angTriang;
    } else if (dx <= 0 && dy >= 0) {
        anguloNorm = M_PI - angTriang;
    } else if (dx <= 0 && dy <= 0) {
        anguloNorm = M_PI + angTriang;
    } else {
        anguloNorm = 2 * M_PI - angTriang;
    }
    //cout << "; angNorm: " << rad2deg(anguloNorm) << "°" << endl;

    return anguloNorm;
}


// Devuelve la magnitud de la menor diferencia entre 2 ángulos, teniendo en cuenta que luego de 359° sigue 0°
float restarAngulos(float a1, float a2) {
    if (a1 == a2) return 0;
    if (a1 == 0) return abs(a2);
    if (a2 == 0) return abs(a1);

    //cout << "(restarAngulos) " << rad2deg(a1) << "° - " << rad2deg(a2) << "°; ";

    // ambos ángulos deben ser menores de M_PI
    a1 = normalizar(a1);
    a2 = normalizar(a2);

    float dAng = abs(a1 - a2);
    //cout << "operacion: " << rad2deg(a2) << "° - " << rad2deg(a1) << "° abs=" << rad2deg(dAng) << "°; ";

    if (dAng > M_PI) dAng = 2 * M_PI - dAng;
    //cout << "resultado: " << rad2deg(dAng) << "°" << endl;

    return abs(dAng);
}


// Retorna la magnitud distancia entre 2 puntos
float distanciaPuntos(Point2f p1, Point2f p2) {
    //cout << "(distanciaPuntos) " << p1 << " - " << p2 << endl;
    return sqrt((p2.x - p1.x) * (p2.x - p1.x) + (p2.y - p1.y) * (p2.y - p1.y));
}


// Retorna la mínima distancia de A al segmento entre B y C
float alturaTriangulo(Point2f A, Point2f B, Point2f C) {
    //cout << "alturaTriangulo A(" << A.x << "," << A.y << ")- B(" << B.x << "," << B.y << ")- C(" << C.x << "," << C.y << ")." << endl;
    float a = distanciaPuntos(B, C);
    float b = distanciaPuntos(A, C);
    float c = distanciaPuntos(A, B);
    float s = (a + b + c) / 2;
    //        cout << "alturaTriangulo a(" << a << ")- b(" << b << ")- c(" << c << ")." << endl;
    float area = sqrt(s * (s - a) * (s - b) * (s - c));
    return 2 * area / a;
}


// Devuelve la vecindad de un punto de la imagen binaria (1 solo si es un pixel ocupado)
// Obligatorio recibir por parámetro uchar vecindad[8]; ya que no se puede crear y devolver arreglos
uchar *vecindad(Mat &img, Point pun, uchar vec[]) {
    vec[0] = (img.at<uchar>(pun.y-1, pun.x) == 0) ? 1 : 0;
    vec[1] = (img.at<uchar>(pun.y-1, pun.x+1) == 0) ? 1 : 0;
    vec[2] = (img.at<uchar>(pun.y, pun.x+1) == 0) ? 1 : 0;
    vec[3] = (img.at<uchar>(pun.y+1, pun.x+1) == 0) ? 1 : 0;
    vec[4] = (img.at<uchar>(pun.y+1, pun.x) == 0) ? 1 : 0;
    vec[5] = (img.at<uchar>(pun.y+1, pun.x-1) == 0) ? 1 : 0;
    vec[6] = (img.at<uchar>(pun.y, pun.x-1) == 0) ? 1 : 0;
    vec[7] = (img.at<uchar>(pun.y-1, pun.x-1) == 0) ? 1 : 0;
    return vec;
}

