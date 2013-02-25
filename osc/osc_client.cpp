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
    return GenericSignalHandler::connect(object, mName);
}

void Notifier::invoke( const QVariantList & args )
{
    mClient->send(mFullPath, args);
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
            qDebug() << "OscClient: Subscribing to:" << fullPath;
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

