#ifndef QML_OSC_INTERFACE_HPP_INCLUDED
#define QML_OSC_INTERFACE_HPP_INCLUDED

#include <QObject>
#include <QQmlParserStatus>
#include <QByteArray>
#include <QStringList>
#include <QPointer>

class QmlOscInterface : public QObject, public QQmlParserStatus
{
    Q_OBJECT
    Q_INTERFACES(QQmlParserStatus)
    Q_PROPERTY( QObject* target READ target WRITE setTarget )
    Q_PROPERTY( QString path READ path WRITE setPath )

public:
    QmlOscInterface( QObject * parent = 0):
        QObject(parent)
    {}

    ~QmlOscInterface();

    void setTarget( QObject * object ) { mTarget = object; }
    QObject *target() { return mTarget; }

    void setPath( const QString & path ) { mPath = path.toLatin1(); }
    QString path() const { return QString::fromLatin1(mPath); }

    const QByteArray & rawPath() { return mPath; }

    virtual void componentComplete();

    virtual void classBegin() {}

private slots:

private:

    QPointer<QObject> mTarget;
    QByteArray mPath;
};

#endif // QML_OSC_INTERFACE_HPP_INCLUDED
