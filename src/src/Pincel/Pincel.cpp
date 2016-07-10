#include "Pincel.h"

Pincel::Pincel(Mat &img) {
    imgBase = img;
    imgRecorrido = Mat(img.rows, img.cols, CV_8U, Scalar(255));
    if (DEBUG_ANA) initMatDebug();

    //debugVideo = VideoWriter("videoPrueba.avi", CV_FOURCC('D','I','V','X'), 60, Size(img.cols * 2, img.rows * 2), true );
    //if (debugVideo.isOpened()) cout << "Video abierto!" << endl;

    _agrOrdenEncolado(0, 0,1,7,2,6,3,5,4);
    _agrOrdenEncolado(1, 1,2,0,3,7,4,6,5);
    _agrOrdenEncolado(2, 2,3,1,4,0,5,7,6);
    _agrOrdenEncolado(3, 3,4,2,5,1,6,0,7);
    _agrOrdenEncolado(4, 4,5,3,6,2,7,1,0);
    _agrOrdenEncolado(5, 5,6,4,7,3,0,2,1);
    _agrOrdenEncolado(6, 6,7,5,0,4,1,3,2);
    _agrOrdenEncolado(7, 7,0,6,1,5,2,4,3);

    ultAnguloRecorrido = 0;
    numRecorrido = 0;
    numPixel = 0;
    anguloEstim = 0;
    anguloCambioProm = 0;
    numControl = 0;
}

// Realiza 2 recorridos desde el punto inicial, ambos en direcciones opuestas
bool Pincel::recorrerDoble(Point pIni) {
    recorrerDireccion(pIni, true, true);
    recorrerInverso();

    imgRecorrido.release();
    return true;
}

// Recorre el cuerpo en una misma dirección todo lo posible, iniciando desde pIni
bool Pincel::recorrerDireccion(Point pIni, bool buscarDirProp, bool centrarPuntoIni) {
    if (DEBUGM) {
        cout << endl << "(recorrerTrazo) Inicio de recorrido " << numRecorrido << " en el punto " << pIni << endl << endl; }

    Point puntoPendiente;
    bool recorrer = true;
    imgRecorrido.setTo(Scalar(255));

    if (centrarPuntoIni) centrarEnLinea(pIni);
    puntoPincel = pIni;

    // Con el punto centrado, preparar un ángulo de propagación válido, si existe
    if (buscarDirProp) {
        recorrer = inicializarDireccion();
    }

    if (recorrer) {
        // Si es el primer recorrido, almacenamos la información correspondiente
        if (numRecorrido == 0) {
            infoRecIni.puntoIni = puntoPincel;
            infoRecIni.anguloIni = anguloEstim;
        }

        cuentaInternaControl = 0;
        puntoUltControl = puntoPincel;

        encolarPunto(puntoPincel);
        do {
            puntoPendiente = pendientes.front();
            pendientes.pop_front();
            analizarPunto(puntoPendiente);

            // Revisamos si es necesario inteerrumpir el recorrido y reevaluar la dirección
            /*if (puntosAnaliz.size() > 4) {
                pendientes.clear();
            }*/
        }
        while (pendientes.size() > 0);
        if (DEBUGM) {
            cout << "(recorrerTrazo) Recorrido terminado en " << ultPuntoRecorrido << endl << endl;}

        // Terminamos el recorrido; revisamos si podemos seguir en alguna dirección
        ultPuntoRecorrido = puntoPincel;
        ultAnguloRecorrido = anguloEstim;
        numRecorrido++;

        if (buscarDiscontinuidad(ultPuntoRecorrido)) {
            // Reiniciar recorrido al otro lado de la discontinuidad
            //cout << "(recorrerTrazo) Discontinuidad resuelta! Continuando en" << ultPuntoRecorrido << endl;
            recorrer = recorrerDireccion(ultPuntoRecorrido, false, false);
        }
        else {
            // Intentar reiniciar recorrido con ultAnguloRecorrido en el último ángulo
            //recorrer = recorrerTrazo(puntoPincel, true, false);
        }
    }

    return recorrer;
}

// Realiza un recorrido en sentido inverso al iniical desde el punto inicial
bool Pincel::recorrerInverso() {
    float angOpuesto = anguloOpuesto(infoRecIni.anguloIni);
    initAnguloRecorrido(angOpuesto);
    corregirDirProp(5, 0.1);
    return recorrerDireccion(infoRecIni.puntoIni, false, false);
}

// Analiza el punto, generalmente desencolado de la cola de pendientes
void Pincel::analizarPunto(Point punto) {
    if (DEBUGM) cout << endl << "(analizarPunto) " << punto << endl;

    // Validamos si el punto se debe procesar
    bool procesar = true;
    PuntoAnalizado punAna;
    punAna.codValido = 0;
    punAna.pos = punto;
    punAna.angulo = anguloDifPuntos(puntoPincel, punto);
    punAna.num = numPixel;

    // sólo analizar si el cambio en dirección no supera el cambio máximo
    if (puntoPincel.x != punto.x || puntoPincel.y != punto.y) {
        float resta = restarAngulos(punAna.angulo, anguloEstim);
        if (resta > cambioAngMax) {
            punAna.codValido = 1;
            procesar = false;
            if (DEBUGM) {
             cout << "(analizarPunto) ERROR: angulo " << rad2deg(punAna.angulo) << "° varia demasiado respecto a " << rad2deg(anguloEstim) << endl; }
        }
    }

    // sólo analizar si el punto está cerca de la línea guía
    procesar = procesar && validarDistGuia(punAna);

    if (procesar) {
        if (DEBUGM) {
            cout << "(analizarPunto) Procesando " << punto << endl; }
        marcaTrazoPincel(punto);
        puntosTrazoPin.push_back(punto);

        marcaRecorrido(punto);
        encolarVecinos(punto);
        moverPincel(punto);

        if (DEBUG_ANA) {
            marcaDebug(punto, 1);
            mostrarDebug(true);
        }
    }
    else {
        guardarPunAna(punAna); // Almacenamos

        if (DEBUG_ANA) {
            marcaDebug(punto, 5);}
        if (DEBUGM) {
            unsigned int i;
            cout << "(analizarPunto) Cancelado análisis de " << punto << endl;
            cout << "(analizarPunto) PuntosAnaliz: ";
            for(i = 0; i != puntosAnaliz.size(); i++) {
                cout << puntosAnaliz[i] << ", ";
            }
            cout << endl;
        }
    }

    numPixel++;
}



// agregar a cola de pendientes los vecinos del punto sólo si el pixel no ha sido recorrido antes
// el encolado se hace según el orden seleccionado
// todo pixel encolado se marca en la Mat de recorrido
void Pincel::encolarVecinos(Point punto) {
    Point punVecino;
    uchar vecinos[8], vecinosReco[8];
    vecindad(imgBase, punto, vecinos);
    vecindad(imgRecorrido, punto, vecinosReco);

    uchar i, dir;

    //cout << "(encolarVecinos) Encolando vecinos de " << punto << ": " << (int)imgBase.at<uchar>(punto) << endl;
    //for (i = 0 ; i < 8 ; i++) { cout << (int)vecinos[i] << ","; } cout << endl;
    //for (i = 0 ; i < 8 ; i++) { cout << (int)vecinosReco[i] << ","; } cout << endl;

    for (i = 0 ; i < numPixEncolar ; i++) {
        // Encolamos en el orden descrito por el arreglo
        dir = ordenEncolado[i];
        //cout << "(encolarVecinos) intentando encolar vecino " << (int)dir << endl;
        if (vecinos[dir] == 1 && vecinosReco[dir] == 0) {
            punVecino = moverPosicion(punto, dir);

            // encolar si no está en la img de los trazos almacenados
            if (imgPincel.at<uchar>(punVecino) == 255) {
                encolarPunto(punVecino);
                //cout << "(encolarVecinos) Encolando: " << punVecino << endl;
                if (DEBUG_ANA) marcaDebug(punVecino, 0);
            }
        }
    }
}


// Mueve el pincel lo suficiente para contener al nuevo punto
void Pincel::moverPincel(Point punto) {
    if (punto.x != puntoPincel.x || punto.y != puntoPincel.y) {
        float dist = distanciaPuntos(puntoPincel, punto);

        // las nuevas coordenadas se salen del pincel actual?
        if (dist > radio) {
            float angPunto = anguloDifPuntos(puntoPincel, punto);

            // desplazamos el pincel sólo lo suficiente para tocar el nuevo punto EN SU CENTRO
            float magDespl = dist - radio + 0.5; // magnitud del desplazamiento
            puntoPincel.x += magDespl * cos(angPunto);
            puntoPincel.y += magDespl * sin(angPunto);

            puntoControl(); // Tomamos punto de control si es el caso

            if (DEBUGM) cout << "(moverPincel) movido a: " << puntoPincel << endl;
            if (DEBUG_ANA) marcaDebug(puntoPincel, 3);
        }
    }
}


/**
 * Incrementa el contador de control
 * Si es el caso, marca un punto de control y recalcula las variables de propagación
 */
void Pincel::puntoControl() {
    cuentaInternaControl++;

    if (cuentaInternaControl == periodoControl) {
        short int i;
        cuentaInternaControl = 0;


        float anguloPunto = normalizar(anguloDifPuntos(puntoUltControl, puntoPincel));

        // Almacenamos la muestra de ángulo
        for (i = 4 ; i > 0 ; i--) {
            anguloControl[i] = anguloControl[i - 1];
        }
        anguloControl[0] = anguloPunto;

        estimarSigAngulo();

        puntoUltControl = puntoPincel;
        if (DEBUG_ANA) marcaDebug(puntoUltControl, 6);
        if (DEBUGM) cout << "(puntoControl) nuevo control ang=" << rad2deg(anguloPunto) << "° en " << puntoUltControl << endl;

        // sólo cambiar la recta guía si ya vamos a terminar la anterior
        if (rectaGuia.longitud > 0) {
            float distPuntoFin = distanciaPuntos(puntoUltControl, rectaGuia.puntoFin);
            if (distPuntoFin < radio * 2) {
                if (DEBUGM) cout << "cerca al fin de la guía, corregimos guía." << endl;
                setRectaGuia(0,0,0,0,0);
                corregirDirProp(10, 0.08);
            }
        }
        else corregirDirProp(10, 0.08);

        numControl++;
    }
}



// Estima el siguiente ángulo de propoagación a partir del comportamiento anterior
// Los ángulos deben ser negativos y de magnitud <= 180
void Pincel::estimarSigAngulo() {
    uchar i;
    const float pesos[5] = { 0.3, 0.25, 0.15, 0.15, 0.15 };
    float compX = 0;
    float compY = 0;

    if (DEBUGM) cout << "(estimarSigAngulo) [" << rad2deg(anguloControl[0]) << "°, " << rad2deg(anguloControl[1]) << "°, " << rad2deg(anguloControl[2]) << "°, " << rad2deg(anguloControl[3]) << "°, " << rad2deg(anguloControl[4]) << "°]" << endl;

    // partimos del promedio de los últimos 2 ángulos
    for (i = 0 ; i < 2 ; i++) {
        compX += pesos[i] * cos(anguloControl[i]);
        compY += pesos[i] * sin(anguloControl[i]);
    }
    float angResultado = anguloPunto(Point2f(compX, compY));

    // estimamos la variación de promediar las variaciones anteriores en una tasa de cambio
    const float cambio[4] = { restarAngulos(anguloControl[0], anguloControl[1]), restarAngulos(anguloControl[1], anguloControl[2]), restarAngulos(anguloControl[2], anguloControl[3]), restarAngulos(anguloControl[3], anguloControl[4]) };
    anguloCambioProm = (0.3 * cambio[0] + 0.3 * cambio[1] + 0.2 * cambio[2] + 0.2 * cambio[3]) * 0.3;
    angResultado += anguloCambioProm;
    angResultado = normalizar(angResultado);

    anguloEstim = angResultado;

    //cout << "(estimarSigAngulo) cambioPromedio: " << rad2deg(anguloCambioProm) << "°" << endl;
    if (DEBUGM) cout << "(estimarSigAngulo) resultado: " << rad2deg(angResultado) << "°" << endl;
}


// Actualiza el orden de encolado según la dirección estimada
void Pincel::actOrdenEncolado() {
    // Sólo permitir ángulos positivos normalizados para comparar en el if
    float ang = anguloEstim;
    ang = normalizar(ang);
    if (ang < 0) ang += (2 * M_PI);

    uchar dirEstim = rad2dir(ang);
    //cout << "(actOrdenEncolado) Dirección estimada: " << (int)dirEstim << "; " << rad2deg(ang) << "°" << endl;

    // Seleccionamos el orden nuevo
    ordenEncolado = ordenesEnc[dirEstim];
}



// Agrega un punto al final de la lista de pendientes y lo marca en la matriz de recorrido
void Pincel::encolarPunto(Point punto) {
    pendientes.push_back(punto);
    marcaRecorrido(punto);
}


// Asigna valores a un arreglo de orden de encolado
void Pincel::_agrOrdenEncolado(uchar index, uchar n0, uchar n1, uchar n2, uchar n3, uchar n4, uchar n5, uchar n6, uchar n7) {
    ordenesEnc[index][0] = n0;
    ordenesEnc[index][1] = n1;
    ordenesEnc[index][2] = n2;
    ordenesEnc[index][3] = n3;
    ordenesEnc[index][4] = n4;
    ordenesEnc[index][5] = n5;
    ordenesEnc[index][6] = n6;
    ordenesEnc[index][7] = n7;
}

// Prepara las variables de dirección para iniciar el recorrido en el ángulo dado
void Pincel::initAnguloRecorrido(float angulo) {
    short int i;
    for (i = 0 ; i < 5 ; i++) anguloControl[i] = angulo;
    anguloEstim = angulo;
    actOrdenEncolado();
}

// Guarda la información de un pincel analizado
// Remueve puntos viejos de la deque de analizados
void Pincel::guardarPunAna(PuntoAnalizado punAna) {
    if (puntosAnaliz.size() > 0) {
        if (punAna.num - puntosAnaliz.back().num > 10) {
            puntosAnaliz.clear();
            //cout << "(guardarPunAna) Limpiando puntos." << endl;
        }
    }
    puntosAnaliz.push_back(punAna);
    if (DEBUGM) cout << "(guardarPunAna) " << punAna << endl;

    if (puntosAnaliz.size() > 4) { if (DEBUGM) cout << "*** Preocupante..." << endl; }
}
