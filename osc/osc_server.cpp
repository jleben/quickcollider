#include "osc_server.hpp"
#include "osc_client.hpp"

OscServer::OscServer(const QByteArray &port ):
    mDispatcher(new OscDispatcher(this))
{
    mServerThread = lo_server_thread_new(port.constData(), NULL);

    if (!mServerThread)
        throw BadPortException();

    lo_server_thread_add_method( mServerThread, NULL, NULL, &OscServer::messageHandler, this );

    Q_ASSERT(mInstance == 0);
    mInstance = this;
}

void OscServer::addInterface( QObject *object, const QByteArray & path )
{
    if (!object)
        return;

    OscInterface intf;
    intf.object = object;
    intf.path = path;

    InterfaceList::iterator it = mInterfaces.begin();
    for (it = mInterfaces.begin(); it != mInterfaces.end(); ++it) {
        if (it->object == object) {
            qWarning() << "OscServer: this object already added:" << intf.path;
            return;
        }
    }

    mInterfaces.append(intf);
    mDispatcher->addInterface(intf);
    qDebug() << "OscServer: added object:" << intf.path;
    emit interfaceAdded(intf);
}

void OscServer::removeInterface( QObject *object )
{
    if (!object)
        return;

    OscInterface intf;

    InterfaceList::iterator it = mInterfaces.begin();
    for (it = mInterfaces.begin(); it != mInterfaces.end(); ++it)
    {
        if  (it->object == object) {
            intf = *it;
            mInterfaces.erase(it);
            mDispatcher->removeInterface(intf);
            qDebug() << "OscServer: removed object:" << intf.path;
            emit interfaceRemoved(intf);
            return;
        }
    }
}

OscClient *OscServer::findClient( const OscAddress & address )
{
    foreach (OscClient *client, mClients) {
        if (client->address() == address)
            return client;
    }
    return 0;
}

void OscServer::deleteIfUnsubscribed( OscClient *client )
{
    if (client->subscriptionCount() == 0) {
        mClients.removeAll(client);
        delete client;
        qDebug("OscServer: removed client: %s:%i",
               client->address().host.constData(),
               client->address().port);
    }
}

OscClient * OscServer::clientForAddress( const OscAddress & address )
{
    OscClient *subscriber = findClient( address );
    if (!subscriber) {
        subscriber = new OscClient( address, this);
        mClients.append(subscriber);
        qDebug("OscServer: new client: %s:%i",
               qPrintable(address.host),
               address.port);
    }
    return subscriber;
}

void OscServer::customEvent(QEvent* event)
{
    static QByteArray setPath("/set/");
    static QByteArray invokePath("/invoke/");
    static QByteArray subscribePath("/subscribe");
    static QByteArray unsubscribePath("/unsubscribe");
    static QByteArray unsubscribeAllPath("/unsubscribeAll");

    OscMessage *message = static_cast<OscMessage*>(event);
    QByteArray path = message->path;
    const QVariantList & args = message->args;

    if (path.startsWith(setPath)) {
        QByteArray dispatchPath = path.mid( setPath.length() - 1 );
        mDispatcher->dispatch( OscDispatcher::Property, dispatchPath, args );
    }
    else if (path.startsWith(invokePath)) {
        QByteArray dispatchPath = path.mid( invokePath.length() - 1 );
        mDispatcher->dispatch( OscDispatcher::Method, dispatchPath, args );
    }
    else if (path == subscribePath) {
        if (args.count() < 2) {
            qWarning("OscClient: subscribe action needs at least 2 arguments.");
            return;
        }
        OscClient *subscriber = clientForAddress( message->source );
        QByteArray path = args[0].toByteArray();
        QList<QByteArray> names;
        for (int i = 1; i < args.count(); ++i)
            names << args[i].toByteArray();
        subscriber->subscribe( path, names );
    }
    else if (path == unsubscribePath)
    {
        if (args.count() < 2) {
            qWarning("OscClient: unsubscribe action needs at least 2 arguments.");
            return;
        }
        OscClient *subscriber = findClient( message->source );
        if (subscriber) {
            QByteArray path = args[0].toByteArray();
            QList<QByteArray> names;
            for (int i = 1; i < args.count(); ++i)
                names << args[i].toByteArray();
            subscriber->unsubscribe(path, names);
            deleteIfUnsubscribed(subscriber);
        }
        else {
            qWarning("OscServer: '/unsubscribe' - no client registered: %s:%i",
                     qPrintable(message->source.host),
                     message->source.port);
        }
    }
    else if (path == unsubscribeAllPath)
    {
        if (args.count() < 1) {
            qWarning("OscServer: '/unsubscribeAll' - missing argument: path");
            return;
        }
        OscClient *subscriber = findClient( message->source );
        if (subscriber) {
            subscriber->unsubscribeAll( args[0].toByteArray() );
            deleteIfUnsubscribed(subscriber);
        } else {
            qWarning("OscServer: '/unsubscribeAll' - no client registered: %s:%i",
                     qPrintable(message->source.host),
                     message->source.port);
        }
    }
    else {
        qWarning() << "OscServer: unknown OSC method:" << path;
    }
}
