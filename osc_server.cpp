#include "osc_server.hpp"
#include "osc_client.hpp"

OscServer::OscServer(const QByteArray &port ):
    mDispatcher(new OscDispatcher(this))
{
    mServerThread = lo_server_thread_new(port.constData(), NULL);

    lo_server_thread_add_method( mServerThread, NULL, NULL, &OscServer::messageHandler, this );

    Q_ASSERT(mInstance == 0);
    mInstance = this;
}

void OscServer::addInterface( QObject *object, const QByteArray & path )
{
    OscInterface intf;
    intf.object = object;
    intf.path = path;

    mInterfaces.append(intf);
    qDebug() << "OscServer: added interface:" << intf.path;
    mDispatcher->addInterface(intf);
    emit interfaceAdded(intf);
}

void OscServer::removeInterface( QObject *object )
{
    OscInterface intf;

    InterfaceList::iterator it = mInterfaces.begin();
    for (it = mInterfaces.begin(); it != mInterfaces.end(); ++it)
    {
        if  (it->object == object) {
            intf = *it;
            mInterfaces.erase(it);
            qDebug() << "OscServer: removed interface:" << intf.path;
            break;
        }
    }

    mDispatcher->removeInterface(intf);
    emit interfaceRemoved(intf);
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

void OscServer::customEvent(QEvent* event)
{
    static QByteArray sendPath("/send/");
    static QByteArray subscribePath("/subscribe");
    static QByteArray unsubscribePath("/unsubscribe");
    static QByteArray unsubscribeAllPath("/unsubscribeAll");

    OscMessage *message = static_cast<OscMessage*>(event);
    QByteArray path = message->path;
    const QVariantList & args = message->args;

    if (path.startsWith(sendPath)) {
        QByteArray dispatchPath = path.mid( sendPath.length() - 1 );
        if (dispatchPath.length() < 2) {
            qWarning("OscServer: '/send' path incomplete.");
            return;
        }
        if (args.count() < 1) {
            qWarning("OscServer: '/send' needs at least one argument.");
            return;
        }
        dispatchPath += '/';
        dispatchPath += args[0].toByteArray();
        mDispatcher->dispatch( dispatchPath, args.mid(1) );
    }
    else if (path == subscribePath) {
        OscClient *subscriber = findClient( message->source );
        if (!subscriber) {
            subscriber = new OscClient( message->source, this);
            mClients.append(subscriber);
            qDebug("OscServer: new client: %s:%i",
                   qPrintable(message->source.host),
                   message->source.port);
        }
        subscriber->subscribe( args );
    }
    else if (path == unsubscribePath)
    {
        OscClient *subscriber = findClient( message->source );
        if (subscriber) {
            subscriber->unsubscribe(args);
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
