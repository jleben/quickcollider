#include "osc_server.hpp"
#include "qml_osc_interface.hpp"

void OscDispatcher::addInterface( const OscInterface &intf )
{
    if (intf.path.isEmpty()) {
        qWarning("OscDispatcher: interface path is empty.");
        return;
    }

    const QMetaObject *metaObject = intf.object->metaObject();

    for (int idx = metaObject->propertyOffset(); idx < metaObject->propertyCount(); ++idx)
    {
        QMetaProperty property = metaObject->property(idx);

        QByteArray path = intf.path;
        path += '/';
        path += property.name();

        PropertyNode *node = new PropertyNode;
        node->object = intf.object;
        node->path = path;
        node->property = property;

        addNode(node);
    }

    for (int idx = metaObject->methodOffset(); idx < metaObject->methodCount(); ++idx)
    {
        QMetaMethod method = metaObject->method(idx);
        if (method.access() != QMetaMethod::Private &&
                (method.methodType() == QMetaMethod::Method
                 || method.methodType() == QMetaMethod::Slot))
        {
            QByteArray path = intf.path;
            path += '/';
            path += method.name();

            MethodNode * node = new MethodNode;
            node->object = intf.object;
            node->path = path;
            node->method = method;

            addNode(node);
        }
    }
}

void OscDispatcher::removeInterface(const OscInterface &intf)
{
    DispatchHash::iterator it = mDispatchHash.begin();
    while( it != mDispatchHash.end() )
    {
        if (it.value()->object == intf.object)
            it = mDispatchHash.erase(it);
        else
            ++it;
    }
}

inline static QGenericArgument toGenericArgument( const QVariantList & varList, int index )
{
    return (index < varList.count())
            ? QGenericArgument(varList[index].typeName(), varList[index].data())
            : QGenericArgument();
}

bool OscDispatcher::dispatch( const QByteArray & path, const QVariantList & args )
{
    DispatchHash::const_iterator it = mDispatchHash.find(path);
    if (it == mDispatchHash.end()) {
        qWarning() << "OscDispatcher: path" << path << "not registered.";
        return false;
    }

    const DispatchNode * node = it.value();

    switch (node->type())
    {
    case DispatchNode::Property: {
        if (args.count() < 1) {
            qWarning() << "OscDispatcher: Cannot set property" << path << "without an argument.";
            return false;
        };
        const PropertyNode *pnode = static_cast<const PropertyNode*>(node);
        bool ok = pnode->property.write( pnode->object, args[0] );
        if (!ok)
            qWarning() << "OscDispatcher: Failed to set property:" << path;
        return ok;
    }
    case DispatchNode::Method: {
        const MethodNode *mnode = static_cast<const MethodNode*>(node);
        bool ok = mnode->method.invoke
                ( mnode->object,
                  toGenericArgument( args, 0 ),
                  toGenericArgument( args, 1 ),
                  toGenericArgument( args, 2 ),
                  toGenericArgument( args, 3 ),
                  toGenericArgument( args, 4 ),
                  toGenericArgument( args, 5 ),
                  toGenericArgument( args, 6 ),
                  toGenericArgument( args, 7 ),
                  toGenericArgument( args, 8 ),
                  toGenericArgument( args, 9 ) );
        if (!ok)
            qWarning() << "OscDispatcher: Failed to invoke method:" << path;
        return ok;
    }
    }

    return false;
}
