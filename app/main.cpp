#include "../osc/osc_server.hpp"
#include "../osc/osc_client.hpp"
#include "../osc/qml_osc_interface.hpp"
#include "../gui/model/slider_model.hpp"
#include "../gui/model/multi_slider_model.hpp"
#include "../gui/model/graph_model.hpp"
#include "../gui/widgets/graph_plotter.hpp"
#include "../gui/widgets/oscilloscope.hpp"
#include "../gui/widgets/sf_view.hpp"
#include "../gui/utility/array_layout.hpp"

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
    qmlRegisterType<QuickCollider::ArrayLayout>
            ("QuickCollider", 0, 1, "ArrayLayout");
    qmlRegisterType<QuickCollider::SliderModel>
            ("QuickCollider", 0, 1, "SliderModel");
    qmlRegisterType<QuickCollider::MultiSliderModel>
            ("QuickCollider", 0, 1, "MultiSliderModel");
    qmlRegisterType<QuickCollider::GraphModel>
            ("QuickCollider", 0, 1, "GraphModel");
    qmlRegisterType<QuickCollider::GraphPlotter>
            ("QuickCollider", 0, 1, "GraphPlotter");
    qmlRegisterType<QuickCollider::GraphPlotter2>
            ("QuickCollider", 0, 1, "GraphPlotter2");
    qmlRegisterType<QuickCollider::Oscilloscope>
            ("QuickCollider", 0, 1, "OscilloscopePlotter");
    qmlRegisterType<QuickCollider::SoundFileView>
            ("QuickCollider", 0, 1, "WaveformPlotter");

    OscServer oscServer( serverPort.toLatin1() );
    oscServer.start();

    QQmlEngine engine;
    QQuickView *window = new QQuickView(&engine, 0);
    window->setColor( app.palette().color(QPalette::Window) );
    window->setSource(QUrl::fromLocalFile(scenePath));
    window->setResizeMode(QQuickView::SizeRootObjectToView);
    window->show();

    return app.exec();
}

