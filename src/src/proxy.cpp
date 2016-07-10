#include "proxy.h"

Proxy::Proxy(QObject *parent) : QObject(parent) {
    uchar i;
    for (i = 0 ; i < 4 ; i++) {
        iMarcas[i].num = i;
        iMarcas[i].x = 0;
        iMarcas[i].y = 0;
        iMarcas[i].este = 0;
        iMarcas[i].norte = 0;
        iMarcas[i].cuadranteArr = false;
        iMarcas[i].cuadranteIzq = false;
    }
}

// Devuelve una cadena con la URI para el provider según el cuadrante solicitado de la imagen
QString Proxy::getCoordsUri(int cuadrante) {
    int anchoImg = imgBinaria.cols;
    int altoImg = imgBinaria.rows;
    int x = 0, y = 0, ancho, alto;

    switch (cuadrante) {
    case 0: // arriba izq
        x = 0;
        y = 0;
        ancho = anchoImg / 2;
        alto = altoImg / 2;
        break;
    case 1: // arriba der
        x = anchoImg / 2;
        ancho = anchoImg - x;
        alto = altoImg / 2;
        break;
    case 2: // abajo izq
        x = 0;
        y = altoImg / 2;
        ancho = anchoImg / 2;
        alto = altoImg - y;
        break;
    case 3: // abajo der
        x = anchoImg / 2;
        y = altoImg / 2;
        ancho = anchoImg - x;
        alto = altoImg - y;
        break;
    }
    QString s = QString("%1/%2/%3/%4/").arg(x).arg(y).arg(ancho).arg(alto);
    //qDebug() << s;
    return s;
}

bool Proxy::cargarImagen(QString ruta) {
   // Convertimos ruta a char[] y removemos prefijo "file:///"
    ruta.remove(0, 8);
    QByteArray arrRuta = ruta.toLocal8Bit();
    char* chRuta = arrRuta.data();
    cout << "(cargarImagen) " << chRuta << endl;

    // Cargamos imagen
    Mat imgOrigi = imread(chRuta, CV_LOAD_IMAGE_COLOR);
    if (!imgOrigi.data) {
        cout << "ERROR: No se pudo cargar la imagen." << endl;
        return false;
    }

    cvtColor(imgOrigi, imgOrigi, CV_BGR2GRAY); // a escala de grises
    threshold(imgOrigi, imgBinaria, 200, 255, CV_THRESH_BINARY); // filtro de umbral para obtener la versión binaria

    imgOrigi.release();
    agregarBorde();

    // limpiamos la matriz de resultado de pincel
    if (!imgPincel.empty()) imgPincel.setTo(Scalar(255));
    if (!imgTrazo.empty()) imgTrazo.setTo(Scalar(255));

    iMapa->setInfoImg(imgBinaria.cols, imgBinaria.rows, ruta);

    provider->imgBinaria = imgBinaria;
    return true;
}

// Agrega un borde de 1 px a la imagen binaria sólo si es necesario
// retorna true si agregó borde
bool Proxy::agregarBorde() {
    unsigned short y, x, xMax2;
    unsigned short yMax = imgBinaria.rows;
    unsigned short xMax = imgBinaria.cols;
    bool encontrado = false;
    uchar* fila;

    fila = imgBinaria.ptr<uchar>(0); // fila arriba
    for (x = 0 ; (x < xMax && !encontrado) ; ++x) {
        if (fila[x] == 0) encontrado = true;
    }
    if (!encontrado) {
        fila = imgBinaria.ptr<uchar>(yMax - 1); // fila abajo
        for (x = 0 ; (x < xMax && !encontrado) ; ++x) {
            if (fila[x] == 0) encontrado = true;
        }
    }
    if (!encontrado) {
        for (y = 1 ; (y < yMax && !encontrado) ; ++y) {
            if (imgBinaria.at<uchar>(y, 0) == 0) encontrado = true;
        }
    }
    if (!encontrado) {
        xMax2 = xMax - 1;
        for (y = 1 ; (y < yMax && !encontrado) ; ++y) {
            if (imgBinaria.at<uchar>(y, xMax2) == 0) encontrado = true;
        }
    }

    if (encontrado) {
        copyMakeBorder(imgBinaria, imgBinaria, 1, 1, 1, 1, BORDER_CONSTANT, 255); // agregamos borde de 1px para evitar errores
        iMapa->bordeAgregado = true;
    }

    return encontrado;
}

// numMarca = [0...3]
void Proxy::setPosMarca(int numMarca, int x, int y) {
    iMarcas[numMarca].num = numMarca;
    iMarcas[numMarca].x = x;
    iMarcas[numMarca].y = y;

    qDebug() << "Marca " << numMarca << " en " << x << ", " << y;
    setInfoMarcas();
}

void Proxy::setCoordsMarca(int numMarca, int este, int norte) {
    iMarcas[numMarca].este = este;
    iMarcas[numMarca].norte = norte;

    qDebug() << "Marca " << numMarca << " coords " << este << ", " << norte;
    setInfoMarcas();
}

// Almacena la información de las 4 marcas en el orden correcto (A, B, C, D en orden de lectura)
// Sólo actúa si las 4 marcas han sido ubicadas
void Proxy::setInfoMarcas() {
    uchar pos, i, j;
    if (iMarcas[0].x > 0 && iMarcas[1].x > 0 && iMarcas[2].x > 0 && iMarcas[3].x > 0) {
        // Averiguamos el orden correcto de las marcas
        puntoCoords im[4] = {iMarcas[0], iMarcas[1], iMarcas[2], iMarcas[3]};
        uchar orden[4] = { 0, 1, 2, 3 };
        qDebug() << "(setInfoMarcas) marcas: (" << iMarcas[0].x << "," << iMarcas[0].y << "), ("
                     << iMarcas[1].x << "," << iMarcas[1].y << "), ("
                     << iMarcas[2].x << "," << iMarcas[2].y << "), ("
                     << iMarcas[3].x << "," << iMarcas[3].y << ")";

        // ordenamos respecto a x
        for (j = 0 ; j < 4 ; j++) {
            pos = j;
            for (i = j + 1 ; i < 4 ; i++) { if (im[j].x >= im[i].x) pos = i; }
            if (pos != j) swap(orden[j], orden[pos]);
        }

        im[orden[0]].cuadranteIzq = true;
        im[orden[1]].cuadranteIzq = true;
        im[orden[2]].cuadranteIzq = false;
        im[orden[3]].cuadranteIzq = false;

        // ordenamos respecto a y
        orden[0] = 0; orden[1] = 1; orden[2] = 2; orden[3] = 3;
        for (j = 0 ; j < 4 ; j++) {
            pos = j;
            for (i = j + 1 ; i < 4 ; i++) { if (im[j].y >= im[i].y) pos = i; }
            if (pos != j) swap(orden[j], orden[pos]);
        }

        im[orden[0]].cuadranteArr = true;
        im[orden[1]].cuadranteArr = true;
        im[orden[2]].cuadranteArr = false;
        im[orden[3]].cuadranteArr = false;

        // reordenamos las marcas en el orden deseado (A, B, C, D)
        uchar ord[4];
        for (i = 0 ; i < 4 ; i++) {
            if (im[i].cuadranteIzq && im[i].cuadranteArr) ord[0] = i;
            else if (!im[i].cuadranteIzq && im[i].cuadranteArr) ord[1] = i;
            else if (im[i].cuadranteIzq && !im[i].cuadranteArr) ord[2] = i;
            else ord[3] = i;
        }

        iMapa->setPosPuntosRef(im[ord[0]].x, im[ord[0]].y, im[ord[1]].x, im[ord[1]].y, im[ord[2]].x, im[ord[2]].y, im[ord[3]].x, im[ord[3]].y);
        iMapa->setCoordsPuntosRef(im[ord[0]].este, im[ord[0]].norte, im[ord[1]].este, im[ord[1]].norte, im[ord[2]].este, im[ord[2]].norte, im[ord[3]].este, im[ord[3]].norte);

        qDebug() << "ordenado ABCD:";
        iMapa->imprimir();
   }
}

// Aplica la máscara de recorte almacenada
void Proxy::recortar(int x, int y, int ancho, int alto) {
    qDebug() << "recortar " << x << "," << y << "," << ancho << "," << alto;
    if (x + ancho > imgBinaria.cols) ancho = imgBinaria.cols - x;
    if (y + alto > imgBinaria.rows) alto = imgBinaria.rows - y;

    Mat imgRecorte = imgBinaria(Rect(x, y, ancho - 1, alto - 1)).clone();
    imgBinaria.release();
    imgBinaria = imgRecorte;
    imgRecorte.release();
    agregarBorde();

    iMapa->setInfoRecorte(x, y, x + ancho, y + alto);

    provider->imgBinaria = imgBinaria;

    iMapa->imprimir();
}


void Proxy::limpiarAislados() {
    limpia::exec_limpiarPxAislados(imgBinaria);
    limpia::exec_limpiarTrazosAislados(imgBinaria);
    provider->imgBinaria = imgBinaria;

}

void Proxy::limpiarBordes() {
    limpia::exec_limpiarPxBordes(imgBinaria);
    provider->imgBinaria = imgBinaria;
}

// *** PINCEL

// Inicializa las matrices necesarias para la etapa de pincel
void Proxy::initEtapaPincel() {
    imgTrazo = Mat(imgBinaria.rows, imgBinaria.cols, CV_8U, Scalar(255));
    imgPincel = Mat(imgBinaria.rows, imgBinaria.cols, CV_8U, Scalar(255));
    provider->imgTrazo = imgTrazo;
    provider->imgPincel = imgPincel;
}

void Proxy::marcarTrazo(int x, int y, float radio, uchar numVecinos, float cambioAngMax) {
    cout << "(marcarTrazo)" << x << "," << y << ", " << radio << ", " << (int)numVecinos << ", " << cambioAngMax << endl;

    if (imgPincel.empty()) initEtapaPincel();

    if (imgBinaria.at<uchar>(y,x) == 0 && imgPincel.at<uchar>(y, x) == 255) {
        cuentaPuntosPincel = 0;
        puntoIniPincel.x = x;
        puntoIniPincel.y = y;

        pin = new Pincel(imgBinaria);
        pin->imgPincelTrazo = imgTrazo;
        pin->imgPincel = imgPincel;
        pin->radio = radio;
        pin->guiaLongMin = 6;
        pin->periodoControl = 5;
        pin->puntosAngEstable = 1;
        pin->numPixEncolar = numVecinos;
        pin->cambioAngMax = cambioAngMax;
        pin->maxDistGuia = 1.3 * radio;

        pin->recorrerDoble(Point(x, y));
        provider->imgTrazo = imgTrazo;
    }
}

// Reintentar el último recorrido de trazo
void Proxy::reintentarRecorrido() {
    cout << "reintentarRecorrido" << puntoIniPincel;
    int r = pin->radio;
    short np = pin->numPixEncolar;
    float ca = pin->cambioAngMax;

    imgTrazo.setTo(255);
    marcarTrazo(puntoIniPincel.x, puntoIniPincel.y, r, np, ca);
}


// Descarta imgBinaria y la reemplaza por la imagen de los trazos seleccionados
void Proxy::aislarTrazos() {
    imgBinaria.release();
    imgBinaria = imgPincel.clone();
    delete pin;
    provider->imgBinaria = imgBinaria;
}

// Guarda el trazo actual en la Mat del pincel y limpia la mat de trazos
void Proxy::guardarTrazo() {
    marcarPuntosBW(imgTrazo, imgPincel);
    imgTrazo.setTo(255);

    provider->imgTrazo = imgTrazo;
    provider->imgPincel = imgPincel;
}

// Resetea imgPincel
void Proxy::borrarTrazos() {
    imgTrazo.setTo(255);
    provider->imgTrazo = imgTrazo;
}

// Dibuja una pinta de lapiz o borrador en las Mat, en una posición con el tamaño indicado
void Proxy::pintar(int x, int y, bool rellenar, int tamano) {
    uchar color = (rellenar ? 0 : 255);
    if (tamano == 0) {
        imgBinaria.at<uchar>(y, x) = color;
        if (!rellenar && !imgTrazo.empty()) {
            imgTrazo.at<uchar>(y, x) = color;
        }
    }
    else {
        dibujarCruz(imgBinaria, x, y, color);
        if (!rellenar && !imgTrazo.empty()) {
            dibujarCruz(imgTrazo, x, y, color);
        }
    }
}

// Dibuja una cruz de 3x3 en la Mat
void Proxy::dibujarCruz(Mat &img, int x, int y, uchar color) {
    img.at<uchar>(y, x) = color;
    img.at<uchar>(y, x - 1) = color;
    img.at<uchar>(y, x + 1) = color;
    img.at<uchar>(y - 1, x) = color;
    img.at<uchar>(y + 1, x) = color;
}

// Encuentra el Minimum Bounding Rectangle de una imagen a blanco y negro
Rect Proxy::MBR(Mat &img) {
    unsigned short y, x, x1, y1, x2, y2;
    unsigned short yMax = img.rows, xMax = img.cols;
    uchar* fila;
    bool seguir = true;

    // borde arriba
    for (y = 0 ; (y < yMax) && seguir ; y++) {
        fila = img.ptr<uchar>(y);
        for (x = 0 ; (x < xMax) && seguir ; ++x) {
            if (fila[x] == 0) {
                y1 = y;
                seguir = false;
            }
        }
    }

    // borde abajo
    seguir = true;
    for (y = (yMax - 1) ; (y > 0) && seguir ; y--) {
        fila = img.ptr<uchar>(y);
        for (x = 0 ; (x < xMax) && seguir ; ++x) {
            if (fila[x] == 0) {
                y2 = y;
                seguir = false;
            }
        }
    }

    // borde izq
    seguir = true;
    for (x = 0 ; (x < xMax) && seguir ; ++x) {
        for (y = 1 ; (y < (yMax - 1)) && seguir ; y++) {
            fila = img.ptr<uchar>(y);
            if (fila[x] == 0) {
                x1 = x;
                seguir = false;
            }
        }
    }

    // borde der
    seguir = true;
    for (x = (xMax - 1) ; (x > 0) && seguir ; --x) {
        for (y = 1 ; (y < (yMax - 1)) && seguir ; y++) {
            fila = img.ptr<uchar>(y);
            if (fila[x] == 0) {
                x2 = x;
                seguir = false;
            }
        }
    }

    return Rect(x1, y1, x2 - x1 + 1, y2 - y1 + 1);
}

// Aplica adelgazamiento a imgBinaria
void Proxy::adelgazar(bool desenfocar) {
    // Nos aseguramos de que va a aparecer la imagen de fondo
    if (imgPincel.empty()) {
        imgPincel = imgBinaria.clone();
        provider->imgPincel = imgPincel;
    }

    // recortamos la imagen para aumentar eficiencia
    Rect mbr = MBR(imgBinaria);
    qDebug() << "MBR: " << mbr.x << ", " << mbr.y << " - " << mbr.width << " x " << mbr.height;
    Mat recorte = imgBinaria(mbr);

    // desenfocamos y adelgazamos
    if (desenfocar) {
        applyGaussianBlur(recorte, 9, 0, 0);
    }
    execThinningGuoHall(recorte);

    provider->imgBinaria = imgBinaria;
}


void Proxy::reconstruir(int h) {

    cout << "reconstruir() " << h << endl;
    unsigned int casos = reco::execLineReconstruction(imgBinaria, h);
    // Si se reconstruyó, adelgazar de nuevo
    if (casos > 0) adelgazar(false);    provider->imgBinaria = imgBinaria;
}


void Proxy::vectorizar(float redPuntos, unsigned int suavizado, QString ruta) {
    ruta.remove(0, 8);
    QByteArray arrRuta = ruta.toLocal8Bit();
    char* chRuta = arrRuta.data();
    cout << "vectorizar() " << chRuta << ", " << redPuntos << ", " << suavizado << endl;
    Mat imgVectorizar = imgBinaria.clone(); // el algoritmo modifica la matriz
    vect::execVectorize(imgVectorizar, chRuta, redPuntos, suavizado, iMapa);
    imgVectorizar.release();
}


// Guarda la imgBinaria en la ruta
void Proxy::guardarImagen(QString ruta) {
    ruta.remove(0, 8);
    QByteArray arrRuta = ruta.toLocal8Bit();
    char* chRuta = arrRuta.data();
    cout << "fuardarImagen() " << chRuta << endl;
    imwrite(chRuta, imgBinaria);
}

// Marca todos los pixeles negros de la Mat origen en la Mat destino
// Ambas matrices deben ser CV_8U
void Proxy::marcarPuntosBW(Mat &origen, Mat &destino) {
    unsigned short y, x;
    uchar* fila;
    uchar intensidad;
    unsigned short yMax = origen.rows - 1;
    unsigned short xMax = origen.cols - 1;

    for (y = 1 ; y < yMax ; ++y) {
        fila = origen.ptr<uchar>(y);
        for (x = 1 ; x < xMax ; ++x) {
            intensidad = fila[x];
            if (intensidad == 0) {
                destino.at<uchar>(y, x) = 0;
            }
        }
    }
}

// Marca todos los pixeles negros de la Mat origen en la Mat destino
// Origen es CV_8U, Destino es CV_8UC3
void Proxy::marcarPuntosCol(Mat &origen, Mat &destino, Vec3b color) {
    unsigned short y, x;
    uchar* fila;
    uchar intensidad;
    unsigned short yMax = origen.rows - 1;
    unsigned short xMax = origen.cols - 1;

    for (y = 1 ; y < yMax ; ++y) {
        fila = origen.ptr<uchar>(y);
        for (x = 1 ; x < xMax ; ++x) {
            intensidad = fila[x];
            if (intensidad == 0) {
                destino.at<Vec3b>(y, x) = color;
            }
        }
    }
}
