#include <QApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include "infomapa.h"
#include "proxy.h"
#include "opencvimgprovider.h"


int main(int argc, char *argv[])
{
    Proxy miProxy;
    InfoMapa iMapa;
    QApplication app(argc, argv);

    QQmlApplicationEngine engine;
    engine.rootContext()->setContextProperty("miProxy", &miProxy);
    engine.rootContext()->setContextProperty("iMapa", &iMapa);

    OpencvImgProvider *imageProvider = new OpencvImgProvider();
    miProxy.provider = imageProvider;
    miProxy.iMapa = &iMapa;

    engine.addImageProvider(QLatin1String("imagen"), imageProvider);

    engine.load(QUrl("qrc:/main.qml"));

    return app.exec();
}
