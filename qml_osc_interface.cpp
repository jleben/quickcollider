#include "qml_osc_interface.hpp"
#include "osc_server.hpp"

void QmlOscInterface::componentComplete()
{
    OscServer::instance()->addInterface(this, mPath);

    /*
  QObject * target = mTarget;
  if (!target)
    target = parent();
  if (target) {
    OscServer::instance()->addInterface(target, mPath);
    connect(target, SIGNAL(destroyed(QObject*)),
            OscServer::instance(), SLOT(removeInterface(QObject*)));
  }
  */
}

QmlOscInterface::~QmlOscInterface()
{
    OscServer::instance()->removeInterface(this);

    /*
  QObject * target = mTarget;
  if (target)
    OscServer::instance()->removeInterface(target);
*/
}
