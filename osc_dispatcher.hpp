#ifndef OSC_DISPATCHER_HPP_INCLUDED
#define OSC_DISPATCHER_HPP_INCLUDED

#include "util.hpp"

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

    Type type() const { return mType; }

    QByteArray path;
    QObject *object;

protected:
    DispatchNode(Type type): mType(type), object(0) {}

private:
    const Type mType;
};

struct PropertyNode : public DispatchNode
{
    PropertyNode(): DispatchNode(Property) {}
    QMetaProperty property;
};

struct MethodNode : public DispatchNode
{
    MethodNode(): DispatchNode(Method) {}
    QMetaMethod method;
};

class OscDispatcher : public QObject
{
    Q_OBJECT

public:
    OscDispatcher(QObject * parent = 0): QObject(parent) {}

    void addInterface( const OscInterface & );
    void removeInterface( const OscInterface & );
    bool dispatch( const QByteArray & path, const QVariantList & args );

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
#if 0
    bool remove( const QByteArray & path )
    {
        int removedCount = sInstance->mDispatchHash.remove(path);
        if (removedCount == 0) {
            qWarning() << "OscDispatcher: cannot unregister path: " << path << "not registered.";
            return false;
        } else {
            qDebug() << "OscDispatcher: unregistered path:" << path;
            return true;
        }
    }
#endif
    DispatchHash mDispatchHash;
};

#endif // OSC_DISPATCHER_HPP_INCLUDED
