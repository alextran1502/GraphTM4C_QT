#ifndef UDP_CLIENT_H
#define UDP_CLIENT_H

#include <QObject>
#include <QUdpSocket>

class UDP_Client : public QObject
{
    Q_OBJECT
public:
    explicit UDP_Client(QObject *parent = nullptr);

signals:
    void udp_packet_available();
};

// https://github.com/rookiepeng/socket-test-qt
#endif // UDP_CLIENT_H
