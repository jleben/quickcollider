#include "qml_osc_interface.hpp"
#include "osc_server.hpp"

QmlOscObject::~QmlOscObject()
{
    if (mTarget)
        OscServer::instance()->removeInterface(mTarget);
}

void QmlOscObject::setPath( const QString & path )
{
    if (!mPath.isEmpty()) {
        //qWarning("QmlOscObject: Changing path not supported yet.");
        return;
    }

    mPath = path.toLatin1();
    mTarget = parent();

    if (!mTarget) {
        qWarning("QmlOscObject: Need a parent object.");
        return;
    }

    OscServer::instance()->addInterface(mTarget, mPath);
    connect(mTarget, SIGNAL(destroyed()), this, SLOT(onTargetDestroyed()));
}

void QmlOscObject::onTargetDestroyed()
{
    OscServer::instance()->removeInterface(mTarget);
    mTarget = 0;
}
