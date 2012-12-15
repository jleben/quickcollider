#include <QDebug>
#include <QObject>
#include <QMetaObject>
#include <QMetaMethod>
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

  bool connect( QObject *origin, const QMetaMethod & signal )
  {
    Q_ASSERT(signal.isValid());

    if (isConnected()) {
      qWarning("GenericSignalHandler: Cannot connect more than once.");
      return false;
    }

    int slotId = QObject::staticMetaObject.methodCount();
    int sigId = signal.methodIndex();

    if( !QMetaObject::connect( origin, sigId, this, slotId,
                               Qt::DirectConnection, 0) )
    {
      qWarning( "GenericSignalHandler: Failed to connect." );
      return false;
    }

    mArgTypes.clear();

    QList<QByteArray> params = signal.parameterTypes();
    for( int i = 0; i < params.count(); ++i ) {
      int type = QMetaType::type( params.at(i).constData() );
      /*if( type == QMetaType::Void )
        qcErrorMsg( QString("QObject:connect: Don't know how to handle '%1', "
                            "use qRegisterMetaType to register it.")
                    .arg(params.at(i).constData()) );*/
      mArgTypes << type;
    }

    mOrigin = origin;

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
      args.reserve( mArgTypes.count() );

      for (int i = 0; i < mArgTypes.count(); ++i) {
        QMetaType::Type type = static_cast<QMetaType::Type>(mArgTypes.at(i));
        args << QVariant( type, argData[i + 1] );
      }

      invoke( args );

      methodId = -1;
    }

    return methodId;
  }

  bool isConnected () const
  {
    // FIXME
    return !mOrigin.isNull();
  }

protected:

  virtual void invoke( const QVariantList & args ) = 0;

  QPointer<QObject> mOrigin;

  QList<int> mArgTypes;
};
