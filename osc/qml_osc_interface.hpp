#ifndef QML_OSC_INTERFACE_HPP_INCLUDED
#define QML_OSC_INTERFACE_HPP_INCLUDED

#include <QObject>
#include <QQmlParserStatus>
#include <QByteArray>
#include <QStringList>

class QmlOscInterface : public QObject, public QQmlParserStatus
{
    Q_OBJECT
    Q_INTERFACES(QQmlParserStatus)
    Q_PROPERTY( QString path READ path WRITE setPath )
    Q_PROPERTY( QObject * target READ target WRITE setTarget )

public:
    QmlOscInterface( QObject * parent = 0):
        QObject(parent),
        mTarget(0)
    {}

    ~QmlOscInterface();

    void setPath( const QString & path );
    QString path() const { return QString::fromLatin1(mPath); }

    void setTarget( QObject * );
    QObject * target() const { return mTarget; }

    const QByteArray & rawPath() { return mPath; }

    virtual void componentComplete();

    virtual void classBegin() {}

private slots:
    void onTargetDestroyed();

private:
    QByteArray mPath;
    QObject *mTarget;
};

#endif // QML_OSC_INTERFACE_HPP_INCLUDED
