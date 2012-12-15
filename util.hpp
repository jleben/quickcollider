#ifndef QML_OSC_UTIL_HPP_INCLUDED
#define QML_OSC_UTIL_HPP_INCLUDED

#include <lo/lo.h>
#include <cstdlib>
#include <QByteArray>
#include <QObject>

struct OscAddress
{
    OscAddress(): port(-1) {}
    OscAddress(lo_address address):
        port( std::atoi(lo_address_get_port(address)) ),
        host( lo_address_get_hostname(address) )
    {}

    lo_address toLoAddress() const
    {
        return lo_address_new ( host.constData(), QByteArray::number(port).constData() );
    }

    bool operator == (const OscAddress & other) const
    {
        return (port == other.port) && (host == other.host);
    }

    int port;
    QByteArray host;
};

struct OscInterface
{
    QObject *object;
    QByteArray path;
};

#endif // QML_OSC_UTIL_HPP_INCLUDED
