#include "osc_server.hpp"
#include "qml_osc_interface.hpp"

void OscDispatcher::addInterface( const OscInterface &intf )
{
    if (intf.path.isEmpty()) {
        qWarning("OscDispatcher: interface path is empty.");
        return;
    }

    DispatchNode *node = new DispatchNode;
    node->object = intf.object;
    node->path = intf.path;
    addNode(node);
}

void OscDispatcher::removeInterface(const OscInterface &intf)
{
    DispatchHash::iterator it = mDispatchHash.find(intf.path);
    if ( it != mDispatchHash.end() ) {
        delete it.value();
        it = mDispatchHash.erase(it);
        qDebug() << "OscDispatcher: removed node for path:" << intf.path;
    }
    else {
        qWarning() << "OscDispatcher: no interface to remove for path:" << intf.path;
    }
}

inline static QGenericArgument toGenericArgument( const QVariantList & varList, int index )
{
    return (index < varList.count())
            ? QGenericArgument(varList[index].typeName(), varList[index].data())
            : QGenericArgument();
}

bool OscDispatcher::dispatch( DispatchTarget targetType,
                              const QByteArray & path, const QVariantList & args )
{
    if (args.count() < 1) {
        qWarning() << "OscDispatcher: Cannot dispatch - need at least 1 argument:" << path;
        return false;
    }

    const QByteArray & nodePath = path;
    QByteArray targetName = args[0].toByteArray();
/*
    int lastSeparatorIdx = path.lastIndexOf('/');
    nodePath = path.left(lastSeparatorIdx);
    targetName = path.mid(lastSeparatorIdx+1);
*/

    if (nodePath.isEmpty() || targetName.isEmpty()) {
        qWarning() << "OscDispatcher: Cannot dispatch - no path or target name:" << path;
        return false;
    }

    DispatchHash::const_iterator it = mDispatchHash.find(path);
    if (it == mDispatchHash.end()) {
        qWarning() << "OscDispatcher: no object for path:" << nodePath;
        return false;
    }

    QObject *object = it.value()->object;
    const QMetaObject *metaObject = object->metaObject();

    bool success;
    if (targetType == Property ) {
        if (args.count() < 2) {
            qWarning() << "OscDispatcher: Cannot set property without a value.";
            return false;
        }
        int propertyIdx = metaObject->indexOfProperty(targetName);
        if (propertyIdx == -1) {
            qWarning() << "OscDispatcher: No property for path:" << (path+'/'+targetName);
            return false;
        }
        success = metaObject->property(propertyIdx).write( object, args[1] );
        if (!success)
            qWarning() << "OscDispatcher: Failed to set property for path:" << (path+'/'+targetName);
    }
    else if (targetType == Method) {
        int methodIdx = QuickCollider::indexOfMethod( metaObject, targetName );
        if (methodIdx == -1) {
            qWarning() << "OscDispatcher: No method for path:" << (path+'/'+targetName);
            return false;
        }
        success = metaObject->method(methodIdx).invoke
                ( object,
                  toGenericArgument( args, 1 ),
                  toGenericArgument( args, 2 ),
                  toGenericArgument( args, 3 ),
                  toGenericArgument( args, 4 ),
                  toGenericArgument( args, 5 ),
                  toGenericArgument( args, 6 ),
                  toGenericArgument( args, 7 ),
                  toGenericArgument( args, 8 ),
                  toGenericArgument( args, 9 ),
                  toGenericArgument( args, 10 ) );
        if (!success)
            qWarning() << "OscDispatcher: Failed to invoke method for path:" << (path+'/'+targetName);
    }

    return success;
}
