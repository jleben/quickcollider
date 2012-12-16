#include "../osc/osc_server.hpp"
#include "../osc/osc_client.hpp"
#include "../osc/qml_osc_interface.hpp"

#include <QGuiApplication>
#include <QQuickView>
#include <QQuickItem>
#include <QQmlContext>
#include <QQmlEngine>
#include <QDebug>

using namespace std;

OscServer * OscServer::mInstance = 0;

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    QStringList arguments = app.arguments();

    if (arguments.count() < 3) {
        qDebug("Usage: quickcollider <OSC port> <QML file>");
        return 1;
    }
    QString serverPort = arguments[1];
    QString scenePath = arguments[2];

    qmlRegisterType<QmlOscInterface>("OSC", 0, 1, "OSC");

    OscServer oscServer( serverPort.toLatin1() );
    oscServer.start();

    QQmlEngine engine;
    QQuickView *window = new QQuickView(&engine, 0);
    window->setSource(QUrl::fromLocalFile(scenePath));
    window->show();

    return app.exec();
}

