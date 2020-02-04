#ifndef UDP_CLIENT_H
#define UDP_CLIENT_H

#include <QObject>
#include <QUdpSocket>

class UDP_Client : public QObject
{
    Q_OBJECT
public:
    explicit UDP_Client(QObject *parent = nullptr);
    bool bind_port(QString ip_address, quint16 port);
    void closeUDP();

signals:
    void udp_packet_available(const QString msg);

public slots:
    void ready_read();

private:
    QUdpSocket * udp_socket;
};

// https://github.com/rookiepeng/socket-test-qt
#endif // UDP_CLIENT_H
