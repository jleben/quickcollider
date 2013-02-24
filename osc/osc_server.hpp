#ifndef OSC_SERVER_HPP_INCLUDED
#define OSC_SERVER_HPP_INCLUDED

#include "osc_dispatcher.hpp"
#include "../utility/utility.hpp"

#include <QDebug>
#include <QObject>
#include <QEvent>
#include <QVariant>
#include <QVector>
#include <QCoreApplication>

#include <lo/lo.h>

class OscClient;

struct OscMessage : public QEvent
{
    OscMessage(): QEvent( (QEvent::Type) (QEvent::User + 1) ) {}
    QByteArray path;
    QVariantList args;
    OscAddress source;
};

class OscServer : public QObject
{
    Q_OBJECT

public:
    typedef QList<OscInterface> InterfaceList;

    static OscServer *instance() { return mInstance; }

    OscServer( const QByteArray & port );

    virtual ~OscServer()
    {
        lo_server_thread_free(mServerThread);
    }

    int port() const { return lo_server_thread_get_port(mServerThread); }

    void start() {
        if (lo_server_thread_start(mServerThread))
            qWarning("OscServer: Failed to start.");
    }

    void stop() {
        if (lo_server_thread_stop(mServerThread))
            qWarning("OscServer: Failed to stop.");
    }

    void sendReply ( const QByteArray & path, const QVariantList & args )
    {
        emit reply (path, args);
    }

    const InterfaceList & interfaces() { return mInterfaces; }
    void addInterface( QObject *, const QByteArray & path );

    OscClient *findClient( const OscAddress & );

public slots:
    void removeInterface( QObject * );

signals:
    void reply ( const QByteArray & path, const QVariantList & args );
    void interfaceAdded ( const OscInterface & );
    void interfaceRemoved ( const OscInterface & );

private:

    void deleteIfUnsubscribed( OscClient * );
    OscClient * clientForAddress( const OscAddress & );

    virtual void customEvent(QEvent* event);

    static QVariant convertArgument ( char type, lo_arg *arg )
    {
        QVariant var;
        switch (type) {
        case 'f':
            var = arg->f;
            break;
        case 'i':
            var = arg->i;
            break;
        case 's':
            var = QByteArray(&arg->s);
            break;
        default:
            qWarning() << "Dispatch: Argument of type" << type << "not converted.";
        }
        return var;
    }

    static int messageHandler(const char *path, const char *types,
                              lo_arg **argv, int argc,
                              lo_message lo_msg, void *user_data)
    {
        OscMessage *message = new OscMessage;
        message->path = path;
        message->source = lo_message_get_source(lo_msg);

        for (int idx = 0; idx < argc; ++idx)
            message->args << convertArgument(types[idx], argv[idx]);

        OscServer *me = reinterpret_cast<OscServer*>(user_data);
        QCoreApplication::postEvent(me, message);

        return 0;
    }

    lo_server_thread mServerThread;

    OscDispatcher *mDispatcher;

    InterfaceList mInterfaces;

    typedef QList<OscClient*> ClientList;
    ClientList mClients;

    static OscServer *mInstance;
};

#endif // OSC_SERVER_HPP_INCLUDED
