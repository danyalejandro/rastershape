#include "Pincel.h"

/**
 * Valida la distancia a la recta guía proyectada (si existe)
 * @return TRUE si no se aleja de la recta demasiado o si la recta ya no debe aplicarse
 */
bool Pincel::validarDistGuia(PuntoAnalizado &punAna) {
    //cout << "(validarDistGuia) validando " << punto << " - ";

    if (rectaGuia.longitud == 0) {
        if (DEBUGM) cout << "(validarDistGuia) guía no proyectada." << endl;
        return true; // guía no proyectada
    }

    // si estamos cerca del final de la recta, no hacemos la comprobación porque se acerca un cambio brusco de dirección
    float distPuntoFin = distanciaPuntos(puntoPincel, rectaGuia.puntoFin);
    if (distPuntoFin < radio) {
        if (DEBUGM) cout << "(validarDistGuia) pincel " << puntoPincel << " cerca al fin de la guía " << rectaGuia.puntoFin << "." << endl;
        return true;
    }

    // si ya pasamos el final de la guía (punto final en dirección opuesta a la de propagación), la cancelamos
    if (distPuntoFin > radio) {
        if (restarAngulos(anguloDifPuntos(punAna.pos, rectaGuia.puntoFin), anguloEstim) > M_PI_2) {
            if (DEBUGM) cout << "(validarDistGuia) Guía ya terminada " << punAna.pos << "; " << rectaGuia.puntoFin << ". Eliminada." << endl;
            setRectaGuia(0, 0, 0, 0, 0);
            return true;
        }
    }

    float distGuia = alturaTriangulo(punAna.pos, rectaGuia.puntoIni, rectaGuia.puntoFin);
    if (distGuia > maxDistGuia) {
        if (DEBUGM) cout << "(validarDistGuia) ERROR: distGuia=" << distGuia << endl;
        punAna.codValido = 2;
        return false;
    }

    if (DEBUGM) cout << "(validarDistGuia) distancia ok." << endl;
    return true;
}



/**
 * Intenta obtener un ángulo alrededor del ángulo estimado actual que proyecte una recta guía de suficiente longitud
 * Puede actualizar rectaGuia si el ángulo mejora
 */
void Pincel::corregirDirProp(uchar intentos, float incremento) {
    float xf, yf, x0, y0, mag, angMax = 0, magMax = 0, angNuevo, cambioAngulo, distPuntoFinal, distFinGuia;
	float xfMin = 1, yfMin = 1, xfMax = imgBase.cols - 2, yfMax = imgBase.rows - 2;
	char sentido;
    bool incrementar, angValido, mejorAngulo = false;
    uchar numVacios, maxVacios = 1;
    Point2f vecDir, vecDirMax;

    sentido = (anguloCambioProm > 0) ? 1 : -1;
    cambioAngulo = sentido * incremento;
    incrementar = false; // true para aplicar incremento

    x0 = puntoPincel.x + 0.5; // En el centro del pixel para más precisión
    y0 = puntoPincel.y + 0.5;

    if (DEBUGM) cout << "(corregirDirProp) Intentando corregir dir prop..." << endl;

    do {
        angNuevo = normalizar(anguloEstim + sentido * cambioAngulo);
        angulo2vector(angNuevo, vecDir); // Construimos un vector unitario

        // Proyectamos magnitud hasta que encuentre un pixel en blanco o lleguemos a los límites de la imagen
        mag = 0;
        numVacios = 0;
        do {
            mag++;
            xf = x0 + mag * vecDir.x;
            yf = y0 + mag * vecDir.y;
            if (imgBase.at<uchar>(yf, xf) == 255) {
                numVacios++;
            }
        } while (numVacios < maxVacios && xf > xfMin && xf < xfMax && yf > yfMin && yf < yfMax);

        if (DEBUGM) cout << "(corregirDirProp) anguloNuevo=" << rad2deg(angNuevo) << "° magnitud=" << mag << endl;

        // Si superamos la mínima mag, almacenamos los valores y reducimos los intentos restantes a 2 (los de por si acaso)
        if (mag > guiaLongMin) {
            if (mag > magMax) {
                angMax = angNuevo;
                magMax = mag;
                vecDirMax = vecDir;
                if (DEBUGM) cout << "(corregirDirProp) min mag superada en " << rad2deg(angMax) << "° - " << magMax << endl;
                if (intentos > 4) intentos = 4;
            }
        }

        if (incrementar) cambioAngulo += incremento;
        incrementar = !incrementar;
        sentido = -sentido;
        intentos--;
    } while (intentos > 0);

    angValido = (magMax > guiaLongMin); // es válido si logramos superar la magnitud mínima
    mejorAngulo = false;

    if (angValido) {
        // Almacenamos resultados y reducimos la magnitud porque siempre nos pasamos
        magMax -= 2;
        xf = x0 + magMax * vecDirMax.x;
        yf = y0 + magMax * vecDirMax.y;
        if (DEBUGM) cout << "(corregirDirProp) recta valida propuesta en " << rad2deg(angMax) << "° - " << magMax << endl;

        // será mejor que el ya existente?
        if (rectaGuia.longitud > 0) {
            distPuntoFinal = distanciaPuntos(puntoUltControl, Point2f(xf, yf));
            distFinGuia = distanciaPuntos(puntoUltControl, rectaGuia.puntoFin);
            mejorAngulo = (distPuntoFinal > distFinGuia);

            if (DEBUGM) cout << "(corregirDirProp) Magnitudes: distPuntoFinal=" << distPuntoFinal << ", distFinGuia=" << distFinGuia << endl;
        }
        else mejorAngulo = true;
    }

    if (mejorAngulo) {
        // Si es mejor, almacenamos el ángulo y la recta guía
        anguloEstim = angMax;
        anguloControl[0] = angMax;
        actOrdenEncolado();

        setRectaGuia(x0, y0, xf, yf, magMax);
    }
    else {
        // Si no es mejor, decidimos si continuar con la guía actual o removerla hasta encontrar una adecuada
        if (restarAngulos(anguloDifPuntos(puntoUltControl, rectaGuia.puntoFin), anguloEstim) > M_PI_2) {
            setRectaGuia(0, 0, 0, 0, 0);
            if (DEBUGM) cout << "(corregirDirProp) Guía eliminada." << endl;
        }
        else {
            if (DEBUGM) cout << "(corregirDirProp) Conservamos guía actual." << endl;
        }
    }
}


void Pincel::setRectaGuia(float x0, float y0, float xMax, float yMax, float l) {
    rectaGuia.puntoIni.x = x0;
    rectaGuia.puntoIni.y = y0;
    rectaGuia.puntoFin.x = xMax;
    rectaGuia.puntoFin.y = yMax;
    rectaGuia.longitud = l;

    if (DEBUGM) cout << "(setRectaGuia) Nueva guía de " << rectaGuia.puntoIni << " a " << rectaGuia.puntoFin << endl;
}



// Calcula el ángulo de propagación inicial
bool Pincel::centrarEnLinea(Point &punto) {
    if (DEBUGM) cout << "(centrarPuntoIni) viejas coords: " << punto << endl;

    // recorremos el cuerpo en todas las direcciones y obtenemos el MBR
    getCuerpoPix(punto, cuerpoIni, imgBase, imgRecorrido, 15, 8, 8);

    // estimamos un punto en el centro del MBR
    int xc = round(cuerpoIni.mbr.x + cuerpoIni.mbr.width / 2);
    int yc = round(cuerpoIni.mbr.y + cuerpoIni.mbr.height / 2);
    punto = Point(xc, yc);

    return true;
}


/**
 * Inicializa las variables de ángulo de propagación con una del vector de direcciones de inicio
 * Si no es el primer recorrido del pincel, la dirección deberá ser suficientemente diferente de la última estimada
 * @return TRUE si se logró obtener un ángulo adecuado para iniciar el recorrido
 */
bool Pincel::inicializarDireccion() {
    bool valido;
    float angInicio;
    float anguloAnterior = anguloControl[2];
    float ultAngOpuesto = normalizar(anguloAnterior + M_PI);

    if (DEBUGM) cout << "(inicializarDireccion) numRecorrido: " << numRecorrido << ", anguloAnterior: " << rad2deg(anguloAnterior) << "°, ultAngOpuesto: " << rad2deg(ultAngOpuesto) << endl;

    // Calculamos las posibles direcciones de propagación
    angulosProyeccion(puntoPincel);

    if (dirsInicio.size() == 0) {
        dirsInicio.push_back(anguloCuerpoPix(cuerpoIni)); // Al menos debe haber una dirección disponible
    }

    // Seleccionamos una dirección de propagación válida
    do {
        angInicio = dirsInicio.back();
        dirsInicio.pop_back();
        if (DEBUGM) cout << "(initAnguloInicial) Validando ang propagación: " << rad2deg(angInicio) << "°... ";

        // Validamos el ángulo es diferente al del recorrido anterior y a su opuesto
        if (numRecorrido > 0) {
            valido = (restarAngulos(ultAngOpuesto, angInicio) > M_PI_2) && (restarAngulos(ultAnguloRecorrido, angInicio) > M_PI_2);
        }
        else valido = true;
    }
    while(!valido && dirsInicio.size() > 0);

    /*
    // desde el centro, calculamos la dirección de propagación inicial
    if (angIniValido) {
        cout << "(centrarPuntoIni) Dirección de inicio encontrada." << endl;
        corregirDirProp();

        // Si obtuvimos dir de propagación, ubicamos el punto de inicio en el extremo más conveniente del cuerpo detectado
        // Nota: esto sólo es adecuado si se detectó el extremo del trazo. Hay que re-evaluar si el caso es un punto cualquiera del trazo
        bool masAncho = (cuerpoIni.mbr.width > cuerpoIni.mbr.height);
        uchar dirEstim = rad2dir(anguloEstim);

        switch (dirEstim) {
            case 0: punto = cuerpoIni.pAba; break;
            case 1: punto = (masAncho) ? cuerpoIni.pAba : cuerpoIni.pIzq; break;
            case 2: punto = cuerpoIni.pIzq; break;
            case 3: punto = (masAncho) ? cuerpoIni.pArr : cuerpoIni.pIzq; break;
            case 4: punto = cuerpoIni.pArr; break;
            case 5: punto = (masAncho) ? cuerpoIni.pArr : cuerpoIni.pDer; break;
            case 6: punto = cuerpoIni.pDer; break;
            case 7: punto = (masAncho) ? cuerpoIni.pAba : cuerpoIni.pDer; break;
        }

        cout << "(centrarPuntoIni) nuevas coords: " << punto << endl;
        marcaDebug(punto, 7);
        return true;
    }
    else {
        cout << "(centrarPuntoIni) No se pudo encontrar una dirección de inicio válida." << endl;
    }
    */

    if (valido) initAnguloRecorrido(angInicio);
    // <editor-fold desc="(Debug)">
    if (DEBUGM) {
        if (valido) cout << "válido." << endl;
        else cout << "NO VALIDO." << endl;
    } // </editor-fold>

    return valido;
}

/**
 * Calcula la dirección de un cuerpo de pixeles sin proyectar rectas
 */
float Pincel::anguloCuerpoPix(CuerpoPix cuerpo) {
    // Si no logramos obtener un ángulo válido, inferimos uno desde el cuerpo de pixeles inicial
    // La dirección irá desde el punto inicial hasta el punto extremo más alejado
    if (dirsInicio.size() == 0) {
        float dist[4]; // 0:arr, 1:der, 2:aba, 3:izq
        float distMax, angMax;
        uchar dirMax = 0;
        dist[0] = distanciaPuntos(cuerpo.pIni, cuerpo.pArr);
        dist[1] = distanciaPuntos(cuerpo.pIni, cuerpo.pDer);
        dist[2] = distanciaPuntos(cuerpo.pIni, cuerpo.pAba);
        dist[3] = distanciaPuntos(cuerpo.pIni, cuerpo.pIzq);

        distMax = dist[0];
        if (dist[1] > distMax) { distMax = dist[1]; dirMax = 1; }
        if (dist[2] > distMax) { distMax = dist[2]; dirMax = 2; }
        if (dist[3] > distMax) { distMax = dist[3]; dirMax = 3; }

        switch (dirMax) {
            case 0: angMax = anguloDifPuntos(cuerpo.pIni, cuerpo.pArr); break;
            case 1: angMax = anguloDifPuntos(cuerpo.pIni, cuerpo.pDer); break;
            case 2: angMax = anguloDifPuntos(cuerpo.pIni, cuerpo.pAba); break;
            default: angMax = anguloDifPuntos(cuerpo.pIni, cuerpo.pIzq); break;
        }

        if (DEBUGM) cout << "(anguloCuerpoPix) " << rad2deg(angMax) << "° Deducido a partir del cuerpo de pix ini." << endl;
        return angMax;
    }

    return 0;
}


// Calcula y almacena las posibles direcciones de propagación desde un punto, proyectando rectas
void Pincel::angulosProyeccion(Point punto) {
    float magMin = 8;
    float a, x0, y0, xi, yi, x, y, mag, magMax = 0;
    short int i;

    // No intentar proyectar en un punto vacío
    if (imgBase.at<uchar>(punto) == 255) {
        if (DEBUGM) cout << "ERROR: " << punto << " vacío." << endl;
        return;
    }

    x0 = punto.x;
    y0 = punto.y;

    // Lanzamos rectas guía en todas las direcciones
    for (i = 0 ; i < 16 ; i++) {
        a = i * M_PI_8;

        xi = cos(a);
        yi = sin(a);

        // Proyectamos magnitud hasta que encuentre un pixel en blanco
        mag = 0;
        do {
            mag++;
            x = x0 + mag * xi;
            y = y0 + mag * yi;
        } while (imgBase.at<uchar>(y, x) == 0);

        if (DEBUGM) cout << "(angulosPropagacion) angulo=" << rad2deg(a) << "° magnitud=" << mag << endl;

        // agregamos al vector sólo si supera la magnitud mínima y es diferente a los ya existentes
        if (mag > magMin) {
            if (DEBUGM) cout << "(angulosPropagacion) " << rad2deg(a) << "° Agregado." << endl;
            dirsInicio.push_back(normalizar(a));
        }

        // Almacenamos el ángulo con la magnitud máxima por si ninguna dirección supera la magMin
        if (mag > magMax) {
            magMax = mag;
        }
    }
}

/**
 * Desde el último punto de un recorrido, busca continuar el recorrido saltando discontinuidades
 * @param sigPunto Almacena el punto en el que se puede continuar el recorrido
 * @return TRUE si encontró la continuación del recorrido
 */
/*bool Pincel::buscarDiscontinuidad(Point &sigPunto) {
    Point punBuscar = ultPuntoRecorrido;
    uchar dir = rad2dir(ultAnguloRecorrido);
    uchar i, j, fuerza = 1, numPx = 1, fuerzaMax = 3;
    char incremX, incremY;

    // sólo buscar discontinuidad si estamos lejos del borde de la imagen
    if (punBuscar.x < 10 || punBuscar.x > imgBase.cols - 10 || punBuscar.y < 10 || punBuscar.y > imgBase.rows - 10) return false;

    cout << "(buscarDiscontinuidad) punto: " << punBuscar << ", ang:" << ultAnguloRecorrido << ", dir: " << (int)dir << endl;
    marcaDebug(punBuscar, 0);

    // arriba o abajo
    if (dir == 0 || dir == 4) {
        incremY = (dir == 0) ? -1 : 1;

        for (j = 1 ; j <= fuerzaMax ; j++) {
            punBuscar.y += incremY;
            for (i = 0 ; i < numPx ; i++) {
                cout << "(buscarDiscontinuidad) analizando: " << punBuscar << endl;
                marcaDebug(punBuscar, j);
                mostrarDebug(true);
                if (imgBase.at<uchar>(punBuscar) == 0) {
                    sigPunto = punBuscar;
                    return true;
                }
                punBuscar.x++;
            }
            numPx += 2;
            punBuscar.x -= (numPx - 1);
        }
    }
    // Derecha o izquierda
    else if (dir == 2 || dir == 6) {
        incremX = (dir == 6) ? -1 : 1;

        for (j = 1 ; j <= fuerzaMax ; j++) {
            punBuscar.x += incremX;
            for (i = 0 ; i < numPx ; i++) {
                cout << "(buscarDiscontinuidad) analizando: " << punBuscar << endl;
                marcaDebug(punBuscar, j);
                mostrarDebug(true);
                if (imgBase.at<uchar>(punBuscar) == 0) {
                    sigPunto = punBuscar;
                    return true;
                }
                punBuscar.y++;
            }
            numPx += 2;
            punBuscar.y -= (numPx - 1);
        }
    }
    // Arriba-Der o Arriba-Izq
    else {
        incremX = (dir == 7 || dir == 5) ? -1 : 1;
        incremY = (dir == 5 || dir == 3) ? -1 : 1;

        cout << "(buscarDiscontinuidad) incrementos: " << (int)incremX << ", " << (int)incremY << endl;

        switch (dir) {
            case 1: punBuscar.x++; punBuscar.y--; break;
            case 3: punBuscar.x++; punBuscar.y++; break;
            case 5: punBuscar.x--; punBuscar.y++; break;
            case 7: punBuscar.x--; punBuscar.y--; break;
        }

        for (j = 1 ; j <= fuerzaMax ; j++) {
            for (i = 0 ; i < numPx ; i++) {
                cout << "(buscarDiscontinuidad) analizando: " << punBuscar << endl;
                marcaDebug(punBuscar, j);
                mostrarDebug(true);
                if (imgBase.at<uchar>(punBuscar) == 0) {
                    sigPunto = punBuscar;
                    return true;
                }
                punBuscar.x += incremX;
            }
            punBuscar.x -= incremX;
            punBuscar.y += incremY;

            for (i = 0 ; i < (numPx - 1) ; i++) {
                cout << "(buscarDiscontinuidad) analizando: " << punBuscar << endl;
                marcaDebug(punBuscar, j);
                mostrarDebug(true);
                if (imgBase.at<uchar>(punBuscar) == 0) {
                    sigPunto = punBuscar;
                    return true;
                }
                punBuscar.y += incremY;
            }
            //punBuscar.y -= incremY;


            punBuscar.x -= incremX * (numPx - 1);
            punBuscar.y -= incremY * (numPx + 1);
            numPx++;
        }
    }

    return false;

}*/

/**
 * Desde el último punto de un recorrido, busca continuar el recorrido saltando discontinuidades
 * @param sigPunto Almacena el punto en el que se puede continuar el recorrido
 * @return TRUE si encontró la continuación del recorrido
 */
bool Pincel::buscarDiscontinuidad(Point &sigPunto) {
    Point punBuscar = puntoPincel;
    Point punCentro;
    uchar dir = rad2dir(ultAnguloRecorrido);
    uchar i, j, numPx = 1, fuerzaMax = 2, dist, fX = 1, fY = 1 ;
    char sentido, incremX = 0, incremY = 0;
    bool lineal = true, incrementar = false; // si no es lineal es diagonal

    // sólo buscar discontinuidad si estamos lejos del borde de la imagen
    if (punBuscar.x < 10 || punBuscar.x > imgBase.cols - 10 || punBuscar.y < 10 || punBuscar.y > imgBase.rows - 10) return false;

    // Iniciamos la búsqueda desde el último punto ocupado que se encuentre desde el centro del pincel en dirección dir
    while(imgBase.at<uchar>(punBuscar) == 0) {
        punBuscar = moverPosicion(punBuscar, dir);
    }

    if (DEBUGM) cout << "(buscarDiscontinuidad) punto: " << punBuscar << ", ang:" << ultAnguloRecorrido << ", dir: " << (int)dir << endl;
    if (DEBUG_ANA) marcaDebug(punBuscar, 0);

    switch (dir) {
        case 0: incremX = 1; incremY = 0; fY = 0; break;
        case 1: incremX = -1; incremY = 1; lineal = false; break;
        case 2: incremX = 0; incremY = 1; fX = 0; break;
        case 3: incremX = -1; incremY = -1; lineal = false; break;
        case 4: incremX = -1; incremY = 0; fY = 0; break;
        case 5: incremX = 1; incremY = -1; lineal = false; break;
        case 6: incremX = 0; incremY = -1; fX = 0; break;
        case 7: incremX = 1; incremY = 1; lineal = false; break;
    }

    punCentro = punBuscar;

    for (j = 1 ; j <= fuerzaMax ; j++) {
        punBuscar = punCentro;
        dist = 1;
        sentido = 1;
        incrementar = false;

        for (i = 0 ; i < numPx ; i++) {
            if (DEBUGM) cout << "(buscarDiscontinuidad) analizando: " << punBuscar << endl;
            if (DEBUG_ANA) marcaDebug(punBuscar, j);
            if (DEBUG_ANA) mostrarDebug(true);

            if (imgBase.at<uchar>(punBuscar) == 0) {
                sigPunto = punBuscar;
                return true;
            }

            punBuscar = Point(punCentro.x + sentido * dist * fX * incremX, punCentro.y + sentido * dist * fY * incremY);

            if (lineal) {
                sentido = (sentido == 1) ? -1 : 1;
            }
            else {
                if (fX == 1) { fX = 0; fY = 1; }
                else { fX = 1; fY = 0; }
            }

            if (incrementar) {
                dist++;
                incrementar = false;
            }
            else incrementar = true;
        }
        if (j < 3) numPx += 2;
        punCentro = moverPosicion(punCentro, dir);
    }

    return false;
}
