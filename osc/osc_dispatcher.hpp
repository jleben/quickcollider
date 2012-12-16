#ifndef OSC_DISPATCHER_HPP_INCLUDED
#define OSC_DISPATCHER_HPP_INCLUDED

#include "../utility/utility.hpp"

#include <QDebug>
#include <QObject>
#include <QMetaObject>
#include <QMetaProperty>
#include <QString>
#include <QStringList>
#include <QVariant>

#include <lo/lo.h>

class OscServer;

struct DispatchNode
{
    enum Type {
        Property,
        Method
    };

    QByteArray path;
    QObject *object;
};

class OscDispatcher : public QObject
{
    Q_OBJECT

public:
    enum DispatchTarget {
        Property,
        Method
    };

    OscDispatcher(QObject * parent = 0): QObject(parent) {}

    void addInterface( const OscInterface & );
    void removeInterface( const OscInterface & );
    bool dispatch( DispatchTarget targetType, const QByteArray & path, const QVariantList & args );

private:
    friend class QmlOscInterface;
    typedef QHash<QByteArray, DispatchNode*> DispatchHash;

    bool addNode( DispatchNode * node )
    {
        DispatchHash::iterator it = mDispatchHash.find( node->path );
        if (it != mDispatchHash.end()) {
            qWarning() << "OscDispatcher: path" << node->path << "already exists.";
            return false;
        }

        mDispatchHash.insert( node->path, node );
        qDebug() << "OscDispatcher: registered path:" << node->path;
        return true;
    }

    DispatchHash mDispatchHash;
};

#endif // OSC_DISPATCHER_HPP_INCLUDED
