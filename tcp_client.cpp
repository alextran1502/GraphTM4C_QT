#include "tcp_client.h"
#include <QHostAddress>
#include <QJsonDocument>
#include <QJsonObject>

TCP_client::TCP_client(QObject *parent) : QObject(parent)
{

}

void TCP_client::open_socket(QString ip_addr)
{
    tcp_socket = new QTcpSocket(this);

    connect(tcp_socket, &QTcpSocket::connected, this, &TCP_client::onConnected);

    tcp_socket->connectToHost(QHostAddress(ip_addr), 1824);

    if (!tcp_socket->waitForConnected(5000))
    {
        qDebug() << "Error: " << tcp_socket->errorString();
    }
}

void TCP_client::onConnected()
{
    qDebug() << "TCP CONNECTED";

    connect(tcp_socket, &QTcpSocket::disconnected, this, &TCP_client::onDisconnected);
    connect(tcp_socket, &QTcpSocket::readyRead, this, &TCP_client::readyRead);
    connect(tcp_socket, &QTcpSocket::bytesWritten, this, &TCP_client::bytesWritten);

    tcp_socket->write("Ping");
}


void TCP_client::closeSocketConnection()
{

    tcp_socket->flush();
    tcp_socket->abort();
//    tcp_socket->close();
//    tcp_socket->disconnectFromHost();

}

void TCP_client::onDisconnected()
{
    qDebug() << "TCP DISCONNECTED";

    disconnect(tcp_socket, &QTcpSocket::connected, this, &TCP_client::onConnected);
    disconnect(tcp_socket, &QTcpSocket::disconnected, this, &TCP_client::onDisconnected);
    disconnect(tcp_socket, &QTcpSocket::readyRead, this, &TCP_client::readyRead);
    disconnect(tcp_socket, &QTcpSocket::bytesWritten, this, &TCP_client::bytesWritten);

    tcp_socket->close();
    tcp_socket->disconnectFromHost();

}

void TCP_client::bytesWritten(qint64 bytes)
{
    qDebug() << bytes << " bytes written...";
}

void TCP_client::readyRead()
{
    /* Read TCP data */
    QByteArray received_buffer;
    received_buffer = tcp_socket->readAll();

    /* Convert to JSON */
    QString data = QString(received_buffer);
    QJsonDocument tcp_json = QJsonDocument::fromJson(data.toUtf8());
    QJsonObject tcp_received_data = tcp_json.object();

    quint16 target_udp_port = quint16(tcp_received_data["openUdpPort"].toInt());

    /* Emit */
    if (target_udp_port != 2283) {
        qWarning() << "Wrong Port";
    } else {
        emit set_udp_port(target_udp_port);
    }
}
