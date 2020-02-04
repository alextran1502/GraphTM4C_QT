#ifndef TCP_CLIENT_H
#define TCP_CLIENT_H

#include <QObject>
#include <QTcpSocket>
#include <QDebug>

class TCP_client : public QObject
{
    Q_OBJECT
public:
    explicit TCP_client(QObject *parent = nullptr);
    void open_socket(QString ip_addr);


signals:
    void set_udp_port(quint16 port);


public slots:
    void onConnected();
    void onDisconnected();
    void bytesWritten(qint64 bytes);
    void readyRead();
    void closeSocketConnection();

private:
    QTcpSocket *tcp_socket;
};

#endif // TCP_CLIENT_H
