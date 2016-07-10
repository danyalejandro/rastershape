TEMPLATE = app

QMAKE_CXXFLAGS += -std=c++0x

QT += qml quick widgets
INCLUDEPATH += C://opencv//build//include

LIBS += -LC:\\opencv\\release\\bin \
   libopencv_core249 \
   libopencv_highgui249 \
   libopencv_imgproc249 \

HEADERS += \
        src/limpieza/limpieza.h \
        src/Pincel/Constantes.h \
        src/Pincel/Pincel.h \
        src/Pincel/PuntoAnalizado.h \
        src/Pincel/Recta.h \
        src/Pincel/utileria.h \
        src/thinning/thinning.h \
        src/reconstruction/lineReconstruction.h \
        src/vect/shapelib/shapefil.h \
        src/vect/vect.h \
        src/vect/vect_RDP.h \
        src/vect/vect_catmull.h \
        src/proxy.h \
        src/Mat2Qt.h \
        src/opencvimgprovider.h \
        src/vect/transform.h \
        src/infomapa.h

SOURCES += \
        src/main.cpp \
        src/proxy.cpp \
        src/opencvimgprovider.cpp\
        src/limpieza/limpieza.cpp \
        src/Pincel/Pincel.cpp \
        src/Pincel/PincelMarcas.cpp \
        src/Pincel/PincelTecnicas.cpp \
        src/Pincel/PuntoAnalizado.cpp \
        src/Pincel/utileria.cpp \
        src/thinning/thinning.cpp \
        src/reconstruction/lineReconstruction.cpp \
        src/vect/vect.cpp \
        src/vect/vect_RDP.cpp \
        src/vect/vect_catmull.cpp \
        src/vect/transform.cpp \
        src/infomapa.cpp

RESOURCES += qml/img/img.qrc \
 qml/src/src.qrc


LIBS += -L$$PWD/src/vect/shapelib/ -lshapelib
INCLUDEPATH += $$PWD/src/vect/shapelib
DEPENDPATH += $$PWD/src/vect/shapelib


win32: RC_ICONS = curvas.ico

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Default rules for deployment.
include(deployment.pri)

OTHER_FILES +=



