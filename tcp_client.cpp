#include "tcp_client.h"

TCP_client::TCP_client(QObject *parent) : QObject(parent)
{
    qDebug() << "Init TCP";
    tcp_socket = new QTcpSocket(this);

}
