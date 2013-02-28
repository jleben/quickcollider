#include "../osc/osc_server.hpp"
#include "../osc/osc_client.hpp"
#include "../osc/qml_osc_interface.hpp"
#include "../gui/model/slider_model.hpp"
#include "../gui/model/multi_slider_model.hpp"
#include "../gui/model/graph_model.hpp"
#include "../gui/widgets/graph_plotter.hpp"

#include <QGuiApplication>
#include <QQuickView>
#include <QQuickItem>
#include <QQmlContext>
#include <QQmlEngine>
#include <QDebug>
#include <QPalette>

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

    qmlRegisterUncreatableType<QmlOscInterface>
            ("OSC", 0, 1, "OSC", "Cannot instantiate this object");
    qmlRegisterType<quick_collider::slider_model>
            ("QuickCollider", 0, 1, "SliderModel");
    qmlRegisterType<quick_collider::multi_slider_model>
            ("QuickCollider", 0, 1, "MultiSliderModel");
    qmlRegisterType<QuickCollider::GraphModel>
            ("QuickCollider", 0, 1, "GraphModel");
    qmlRegisterType<QuickCollider::GraphPlotter>
            ("QuickCollider", 0, 1, "GraphPlotter");
    qmlRegisterType<QuickCollider::GraphPlotter2>
            ("QuickCollider", 0, 1, "GraphPlotter2");

    OscServer oscServer( serverPort.toLatin1() );
    oscServer.start();

    QQmlEngine engine;
    QQuickView *window = new QQuickView(&engine, 0);
    window->setColor( app.palette().color(QPalette::Window) );
    window->setSource(QUrl::fromLocalFile(scenePath));
    window->show();

    return app.exec();
}

