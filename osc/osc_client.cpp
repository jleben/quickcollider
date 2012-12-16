#include "osc_client.hpp"
#include "osc_server.hpp"
#include "qml_osc_interface.hpp"

Notifier::Notifier (OscClient *client, const QByteArray & path):
    GenericSignalHandler(client),
    mClient(client),
    mPath(path)
{
    int lastSeparator = path.lastIndexOf('/');
    if (lastSeparator == -1) {
        qWarning("Notifier: invalid path.");
        return;
    }
    mBasePath = path.left(lastSeparator);
    mName = path.mid(lastSeparator+1);
}

void Notifier::invoke( const QVariantList & args )
{
    mClient->send(mPath, args);
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

void OscClient::subscribe ( const QVariantList & args )
{
    if (args.count() < 2) {
        qWarning("OscClient: subscribe action needs at least 2 arguments.");
    }

    QByteArray basePath = args[0].toByteArray();
    QObject *object = 0;

    const OscServer::InterfaceList &interfaces = mServer->interfaces();
    foreach( const OscInterface & intf, interfaces )
    {
        if (intf.path == basePath) {
            object = intf.object;
            break;
        }
    }

    for (int argIdx = 1; argIdx < args.count(); ++argIdx)
    {
        QByteArray name = args[argIdx].toByteArray();
        QByteArray path = basePath + '/' + name;

        Notifier * notifier;
        NotificationHash::iterator it = mNotifications.find(path);
        if (it == mNotifications.end())
        {
            Notifier *notifier = new Notifier(this, path);
            mNotifications.insert(path, notifier);
            if (object)
            {
                // TODO: How to handle signals with same name but different arguments?
                const QMetaObject *metaObject = object->metaObject();
                int signalIdx = QuickCollider::indexOfMethod(metaObject, name );
                if (signalIdx != -1) {
                    if ( notifier->connect( object, metaObject->method(signalIdx) ) )
                        qDebug() << "OscClient: Connected:" << path;
                    else
                        qWarning() << "OscClient:: Failed to connect:" << path;
                }
                else {
                    qWarning("OscClient: interface at \"%s\" has no signal named \"%s\"",
                             basePath.constData(), name.constData());
                }
            }
            qDebug() << "OscClient: subscribed to:" << path;
        }
        else {
            qDebug() << "OscClient: already subscribed to:" << path;
        }
    }
}

void OscClient::unsubscribe( const QVariantList &args )
{
    if (args.count() < 2) {
        qWarning("OscClient: unsubscribe action needs at least 2 arguments.");
    }

    QByteArray basePath = args[0].toByteArray();

    for (int argIdx = 1; argIdx < args.count(); ++argIdx)
    {
        QByteArray name = args[argIdx].toByteArray();
        QByteArray path = basePath + '/' + name;
        NotificationHash::iterator it = mNotifications.find(path);
        if (it != mNotifications.end()) {
            delete it.value();
            mNotifications.erase(it);
            qDebug() << "OscClient: unsubscribed from:" << path;
        }
        else {
            qWarning() << "OscClient: no subscription to remove for:" << path;
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
            qDebug() << "OscClient: unsubscribed from:" << fullPath;
        }
        else {
            ++it;
        }
    }
}

void OscClient::onInterfaceAdded ( const OscInterface & intf )
{
    if (intf.path.isEmpty()) {
        qWarning("OscClient: interface path is empty.");
        return;
    }

    const QMetaObject *metaObject = intf.object->metaObject();

    NotificationHash::iterator it;
    for( it = mNotifications.begin(); it != mNotifications.end(); ++it )
    {
        Notifier *notifier = it.value();
        if (notifier->path() == intf.path) {
            const QByteArray &methodName = notifier->name();
            int signalIdx = QuickCollider::indexOfMethod(metaObject, methodName);
            if (signalIdx == -1) {
                qWarning() << "OscClient: No signal" << methodName
                           << "found for subscription at:" << notifier->path();
                continue;
            }
            if (notifier->connect(intf.object, metaObject->method(signalIdx)))
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

