#ifndef OSC_CLIENT_HPP_INCLUDED
#define OSC_CLIENT_HPP_INCLUDED

#include "../utility/utility.hpp"
#include "../utility/generic_signal_handler.hpp"

#include <lo/lo.h>
#include <QDebug>
#include <QObject>
#include <QMetaMethod>
#include <QVariant>
#include <QHash>
#include <QByteArray>
#include <QPointer>

class OscServer;
class OscClient;
class QmlOscInterface;


class Notifier : protected GenericSignalHandler
{
public:
    Notifier ( OscClient *client, const QByteArray & path, const QByteArray & name );

    const QByteArray & path() const { return mPath; }

    bool connect( QObject *origin );

protected:
    void invoke( const QVariantList & args );

private:
    OscClient *mClient;
    QByteArray mPath;
    QByteArray mFullPath;
    QByteArray mName;
    int mPropertyIdx;
};

class OscClient : public QObject
{
    Q_OBJECT

public:

    OscClient( const OscAddress & address, OscServer * server );

    ~OscClient();

    const OscAddress & address() const { return mAddress; }

    void subscribe ( const QByteArray & path,
                     const QList<QByteArray> & names );

    void unsubscribe ( const QByteArray & path, const QList<QByteArray> & names );

    void unsubscribeAll ( const QByteArray & path );

    int subscriptionCount() const { return mNotifications.count(); }

    void send ( const QByteArray & path, const QVariantList & args );

private slots:

    void onInterfaceAdded ( const OscInterface & );

private:

    static void convertArgument( const QVariant & var, lo_message msg )
    {
        switch (var.userType())
        {
        case QMetaType::Int:
            lo_message_add(msg, "i", var.toInt());
            break;
        case QMetaType::Float:
            lo_message_add(msg, "f", var.toFloat());
            break;
        case QMetaType::Double:
            lo_message_add(msg, "d", var.toDouble());
            break;
        case QMetaType::QString:
            lo_message_add(msg, "s", var.toString().toLatin1().constData());
            break;
        default:
            qWarning() << "Reply: Argument of type" << var.type() << "not converted.";
        }
    }

    OscServer *mServer;
    OscAddress mAddress;
    lo_address mLoAddress;

    typedef QHash<QByteArray, Notifier*> NotificationHash;
    NotificationHash mNotifications;
};

#endif // OSC_CLIENT_HPP_INCLUDED
