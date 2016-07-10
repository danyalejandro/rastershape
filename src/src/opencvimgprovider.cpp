#include "opencvimgprovider.h"

OpencvImgProvider::OpencvImgProvider() : QQuickImageProvider(QQuickImageProvider::Pixmap) {
}

// Retorna un QPixMap a blanco y negro con con la imagen binaria
QPixmap OpencvImgProvider::getImgBinaria(int xMin, int yMin, int xMax, int yMax, int ancho, int alto) {
    QImage qimg = QImage(ancho, alto, QImage::Format_Mono);
    qimg.fill(Qt::color1);

    // Copiamos uno por uno los pixeles
    uchar* fila;
    uchar intensidad;
    unsigned short int x, y;

    for (y = yMin ; y <= yMax ; ++y) {
        fila = imgBinaria.ptr<uchar>(y);
        for (x = xMin ; x <= xMax ; ++x) {
            intensidad = fila[x];
            if (intensidad == 0) {
                qimg.setPixel(x - xMin, y - yMin, Qt::color0);
            }
        }
    }

    return QPixmap::fromImage(qimg, Qt::MonoOnly);
}

// Retorna un QPixmap de 4 canales con la imagen de pincel completa semitransparente verde
QPixmap OpencvImgProvider::getImgPincel(int xMin, int yMin, int xMax, int yMax, int ancho, int alto) {
    QImage qimg = QImage(ancho, alto, QImage::Format_RGBA8888);
    qimg.fill(qRgba(0, 0, 0, 0));

    // Copiamos uno por uno los pixeles
    uchar* fila;
    uchar intensidad;
    unsigned short int x, y;

   for (y = yMin ; y <= yMax ; ++y) {
        fila = imgPincel.ptr<uchar>(y);
        for (x = xMin ; x <= xMax ; ++x) {
            intensidad = fila[x];
            if (intensidad == 0) {
                qimg.setPixel(x - xMin, y - yMin, qRgba(50, 255, 0, 100));
            }
        }
    }

    return QPixmap::fromImage(qimg);
}


// Retorna un QPixmap de 4 canales transparente con los trazos rojos y verdes
QPixmap OpencvImgProvider::getImgTrazos(int xMin, int yMin, int xMax, int yMax, int ancho, int alto) {
    QImage qimg = QImage(ancho, alto, QImage::Format_RGBA8888);
    qimg.fill(qRgba(0, 0, 0, 0));

    // Copiamos uno por uno los pixeles
    uchar* fila;
    uchar intensidad;
    unsigned short int x, y;

   for (y = yMin ; y <= yMax ; ++y) {
        fila = imgPincel.ptr<uchar>(y);
        for (x = xMin ; x <= xMax ; ++x) {
            intensidad = fila[x];
            if (intensidad == 0) {
                qimg.setPixel(x - xMin, y - yMin, qRgba(0, 210, 0, 255));
            }
        }
    }

    // si se solicita la img de trazo, agregar también el trazo actual
    for (y = yMin ; y <= yMax ; ++y) {
        fila = imgTrazo.ptr<uchar>(y);
        for (x = xMin ; x <= xMax ; ++x) {
            intensidad = fila[x];
            if (intensidad == 0) {
                qimg.setPixel(x - xMin, y - yMin, qRgba(255, 0, 0, 255));
            }
        }
    }

    return QPixmap::fromImage(qimg);
}

/**
 * @brief OpencvImgProvider::requestPixmap
 * @param id Identificador de imagen, en formato imagen/x/y/ancho/alto
 * imagen: bin (binaria), trazos (capa superpuesta)
 * @param size
 * @param requestedSize
 * @return
 */
QPixmap OpencvImgProvider::requestPixmap(const QString &id, QSize *size, const QSize &requestedSize) {
    unsigned short imgAncho, imgAlto;

    imgAncho = imgBinaria.cols;
    imgAlto = imgBinaria.rows;

    // Interpretamos la id
    QStringList args = id.split("/");
    QString argImg = args[0];
    unsigned short argX = args[1].toInt();
    unsigned short argY = args[2].toInt();
    unsigned short argAncho = args[3].toInt();
    unsigned short argAlto = args[4].toInt();

    // Validamos los argumentos
    bool valido = true;
    if (argX < 0 || argX >= imgAncho || argX + argAncho > imgAncho
            || argY < 0 || argY >= imgAlto || argY + argAlto > imgAlto) {
        valido = false;
        qDebug() << "(requestPixmap) ERROR: dimensiones inválidas." << endl;
    }
    if (!valido) { return QPixmap(); }

    unsigned short xMax = argX + argAncho - 1;
    unsigned short yMax = argY + argAlto - 1;

    // Seleccionamos la imagen solicitada
    if (argImg == "bin") {
        return getImgBinaria(argX, argY, xMax, yMax, argAncho, argAlto);
    }
    else if (argImg == "trazos") {
        return getImgTrazos(argX, argY, xMax, yMax, argAncho, argAlto);
    }
    else if (argImg == "pincel") {
        return getImgPincel(argX, argY, xMax, yMax, argAncho, argAlto);
    }

}
