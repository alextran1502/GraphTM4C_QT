#include "udp_client.h"

UDP_Client::UDP_Client(QObject *parent) : QObject(parent)
{

}

bool UDP_Client::bind_port(QString ip_address, quint16 port)
{
    udp_socket = new QUdpSocket(this);
    udp_socket->abort();
    bool is_bind = udp_socket->bind(QHostAddress(ip_address), port);

    qDebug() << "Open new UDP" << is_bind;
    if (is_bind)
    {

        connect(udp_socket, &QUdpSocket::readyRead, this, &UDP_Client::ready_read);
    }


    return is_bind;
}

void UDP_Client::closeUDP()
{
    udp_socket->abort();
    disconnect(udp_socket, &QUdpSocket::readyRead, this, &UDP_Client::ready_read);
}

void UDP_Client::ready_read()
{
    QByteArray buffer;
    buffer.resize(udp_socket->pendingDatagramSize());

    QHostAddress sender;
    quint16 sender_port;

    udp_socket->readDatagram(buffer.data(), buffer.size(), &sender, &sender_port);

    emit udp_packet_available(buffer);
}

