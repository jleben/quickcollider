#include "osc_client.hpp"
#include "osc_server.hpp"
#include "qml_osc_interface.hpp"


Notifier::Notifier( OscClient *client, const QByteArray & path, const QByteArray & name ):
    GenericSignalHandler(client),
    mClient(client),
    mPath(path),
    mFullPath( path + '/' + name ),
    mName(name)
{}

bool Notifier::connect( QObject *object )
{
    // TODO: How to handle signals with same name but different arguments?
    const QMetaObject *metaObject = object->metaObject();

    int propertyIdx = metaObject->indexOfProperty( mName.constData() );
    if (propertyIdx != -1)
    {
        int signalIdx = metaObject->property( propertyIdx ).notifySignalIndex();
        if (signalIdx != -1)
        {
            if (GenericSignalHandler::connect( object, metaObject->method(signalIdx) )) {
                mPropertyIdx = propertyIdx;
                return true;
            }
        }
    }
    else
    {
        int signalIdx = QuickCollider::indexOfMethod(metaObject, mName, QMetaMethod::Signal);
        if (signalIdx != -1)
        {
            if (GenericSignalHandler::connect( object, metaObject->method(signalIdx) )) {
                mPropertyIdx = -1;
                return true;
            }
        }
    }

    return false;
}

void Notifier::invoke( const QVariantList & args )
{
    if (mPropertyIdx == -1) {
        mClient->send(mFullPath, args);
    }
    else {
        Q_ASSERT( mOrigin );
        QVariant arg = mOrigin->metaObject()->property( mPropertyIdx ).read( mOrigin );
        QVariantList args;
        args << arg;
        mClient->send(mFullPath, args);
    }
}

OscClient::OscClient( const OscAddress & address, OscServer * server ):
    mServer(server),
    mAddress(address),
    mLoAddress(address.toLoAddress())
{
    QObject::connect(server, SIGNAL(interfaceAdded(OscInterface)),
                     this, SLOT(onInterfaceAdded(OscInterface)));
}

OscClient::~OscClient()
{
    lo_address_free(mLoAddress);
}

void OscClient::subscribe ( const QByteArray & path,
                            const QList<QByteArray> & names )
{
    QObject *object = 0;

    const OscServer::InterfaceList &interfaces = mServer->interfaces();
    foreach( const OscInterface & intf, interfaces )
    {
        if (intf.path == path) {
            object = intf.object;
            break;
        }
    }

    foreach ( const QByteArray & name, names )
    {
        QByteArray fullPath = path + '/' + name;

        Notifier * notifier;
        NotificationHash::iterator it = mNotifications.find(fullPath);
        if (it == mNotifications.end())
        {
            Notifier *notifier = new Notifier(this, path, name);
            mNotifications.insert(fullPath, notifier);
            if (object)
            {
                bool connected = notifier->connect( object );
                if (connected)
                    qDebug() << "OscClient: Connected:" << fullPath;
                else
                    qWarning() << "OscClient: Failed to connect:" << fullPath;
            }
            qDebug() << "OscClient: Subscribed to:" << fullPath;
        }
        else {
            qDebug() << "OscClient: Already subscribed to:" << fullPath;
        }
    }
}

void OscClient::unsubscribe( const QByteArray & path, const QList<QByteArray> & names )
{
    foreach( const QByteArray & name, names )
    {
        QByteArray fullPath = path + '/' + name;
        NotificationHash::iterator it = mNotifications.find(fullPath);
        if (it != mNotifications.end()) {
            delete it.value();
            mNotifications.erase(it);
            qDebug() << "OscClient: Unsubscribed from:" << fullPath;
        }
        else {
            qWarning() << "OscClient: No subscription to remove for:" << fullPath;
        }
    }
}

void OscClient::unsubscribeAll ( const QByteArray & path )
{
    NotificationHash::iterator it = mNotifications.begin();
    while ( it != mNotifications.end() )
    {
        Notifier *notifier = it.value();
        if (notifier->path() == path) {
            QByteArray fullPath = it.key();
            delete notifier;
            it = mNotifications.erase(it);
            qDebug() << "OscClient: Unsubscribed from:" << fullPath;
        }
        else {
            ++it;
        }
    }
}

void OscClient::onInterfaceAdded ( const OscInterface & intf )
{
    if (intf.path.isEmpty()) {
        qWarning("OscClient: Interface path is empty.");
        return;
    }

    const QMetaObject *metaObject = intf.object->metaObject();

    NotificationHash::iterator it;
    for( it = mNotifications.begin(); it != mNotifications.end(); ++it )
    {
        Notifier *notifier = it.value();
        if (notifier->path() == intf.path) {
            bool connected = notifier->connect( intf.object );
            if (connected)
                qDebug() << "OscClient: Connected:" << it.key();
            else
                qWarning() << "OscClient:: Failed to connect:" << it.key();
        }
    }
}

void OscClient::send ( const QByteArray & path, const QVariantList & args )
{
    lo_message msg = lo_message_new();

    foreach( const QVariant & arg, args)
        convertArgument(arg, msg);

    lo_send_message(mLoAddress, path.constData(), msg);

    lo_message_free(msg);
}

