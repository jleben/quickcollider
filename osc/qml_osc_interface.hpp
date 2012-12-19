#ifndef QML_OSC_INTERFACE_HPP_INCLUDED
#define QML_OSC_INTERFACE_HPP_INCLUDED

#include <QObject>
#include <QtQml>
#include <QByteArray>
#include <QStringList>
#include <QDebug>

class QmlOscObject : public QObject
{
    Q_OBJECT
    Q_PROPERTY( QString path READ path WRITE setPath )

public:
    QmlOscObject( QObject * parent = 0):
        QObject(parent),
        mTarget(0)
    {}

    ~QmlOscObject();

    void setPath( const QString & path );
    QString path() const { return QString::fromLatin1(mPath); }

    const QByteArray & rawPath() { return mPath; }

private slots:
    void onTargetDestroyed();

private:
    QByteArray mPath;
    QObject *mTarget;
};

class QmlOscInterface : public QObject
{
    Q_OBJECT
public:
    static QmlOscObject *qmlAttachedProperties(QObject *object)
    {
        return new QmlOscObject(object);
    }
};

QML_DECLARE_TYPEINFO(QmlOscInterface, QML_HAS_ATTACHED_PROPERTIES);

#endif // QML_OSC_INTERFACE_HPP_INCLUDED
