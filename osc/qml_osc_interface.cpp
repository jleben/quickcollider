#include "qml_osc_interface.hpp"
#include "osc_server.hpp"

void QmlOscInterface::componentComplete()
{
    if (!mTarget)
        mTarget = parent();

    if (mTarget && !mPath.isEmpty()) {
        OscServer::instance()->addInterface(mTarget, mPath);
        connect(mTarget, SIGNAL(destroyed()), this, SLOT(onTargetDestroyed()));
    }
}

QmlOscInterface::~QmlOscInterface()
{
    if (mTarget)
        OscServer::instance()->removeInterface(mTarget);
}

void QmlOscInterface::setPath( const QString & path )
{
    if (!mPath.isEmpty()) {
        //qWarning("QmlOscInterface: Invalid attempt to set path when one already set.");
        return;
    }
    mPath = path.toLatin1();
}

void QmlOscInterface::setTarget( QObject *object )
{
    if (mTarget) {
        //qWarning("QmlOscInterface: Invalid attempt to set target when one already set.");
        return;
    }
    mTarget = object;
}

void QmlOscInterface::onTargetDestroyed()
{
    OscServer::instance()->removeInterface(mTarget);
    mTarget = 0;
}
