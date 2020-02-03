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

signals:

private:
    QTcpSocket * tcp_socket;
};

#endif // TCP_CLIENT_H
