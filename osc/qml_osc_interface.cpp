#include "qml_osc_interface.hpp"
#include "osc_server.hpp"

void QmlOscInterface::componentComplete()
{
    OscServer::instance()->addInterface(this, mPath);
}

QmlOscInterface::~QmlOscInterface()
{
    OscServer::instance()->removeInterface(this);
}
