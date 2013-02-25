#include "utility.hpp"

#include <QDebug>
#include <QObject>
#include <QMetaObject>
#include <QMetaMethod>
#include <QQmlProperty>
#include <QPointer>
#include <QVariant>
#include <QByteArray>
#include <QList>

class GenericSignalHandler: public QObject
{
public:

    GenericSignalHandler( QObject * parent = 0 )
        : QObject( parent )
    {}

    bool connect( QObject *object, const QByteArray & name )
    {
        if (is_connected()) {
            qWarning("GenericSignalHandler: Cannot connect more than once.");
            return false;
        }

        int slot_idx = QObject::staticMetaObject.methodCount();

        QQmlProperty qml_property( object, QString::fromLatin1(name) );
        if (qml_property.isProperty() && qml_property.hasNotifySignal())
        {
            qDebug( "GenericSignalHandler: Connecting property...");
            if (!qml_property.connectNotifySignal( this, slot_idx ))
            {
                qWarning( "GenericSignalHandler: Failed to connect property." );
                return false;
            }
            m_arg_types.clear();
            m_qml_property = qml_property;
        }
        else
        {
            const QMetaObject * metaObject = object->metaObject();
            int sig_idx = QuickCollider::indexOfMethod(metaObject, name, QMetaMethod::Signal);
            if (sig_idx == -1) {
                qWarning() << "GenericSignalHandler: No such property or signal:" << name;
                return false;
            }

            qDebug( "GenericSignalHandler: Connecting signal...");

            QMetaMethod signal = metaObject->method(sig_idx);

            if (!QMetaObject::connect( object, sig_idx, this, slot_idx,
                                       Qt::DirectConnection, 0))
            {
                qWarning( "GenericSignalHandler: Failed to connect signal." );
                return false;
            }

            m_qml_property = QQmlProperty();
            m_arg_types.clear();

            QList<QByteArray> params = signal.parameterTypes();
            for( int i = 0; i < params.count(); ++i ) {
                int type = QMetaType::type( params.at(i).constData() );
                m_arg_types << type;
            }
        }

        m_object = object;

        return true;
    }

    int qt_metacall( QMetaObject::Call call, int methodId, void **argData )
    {
        methodId = QObject::qt_metacall( call, methodId, argData );

        if( methodId < 0 )
            return methodId;

        if( call == QMetaObject::InvokeMetaMethod ) {
            Q_ASSERT( methodId == 0 );

            QList<QVariant> args;

            if (m_qml_property.isValid())
            {
                args << m_qml_property.read();
            }
            else
            {
                args.reserve( m_arg_types.count() );
                for (int i = 0; i < m_arg_types.count(); ++i) {
                    QMetaType::Type type = static_cast<QMetaType::Type>(m_arg_types.at(i));
                    args << QVariant( type, argData[i + 1] );
                }
            }

            invoke( args );

            methodId = -1;
        }

        return methodId;
    }

    bool is_connected () const
    {
        // FIXME
        return !m_object.isNull();
    }

protected:

    virtual void invoke( const QVariantList & args ) = 0;

    QPointer<QObject> m_object;
    QQmlProperty m_qml_property;
    QList<int> m_arg_types;
};
