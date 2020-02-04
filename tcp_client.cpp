#include "tcp_client.h"


TCP_client::TCP_client(QObject *parent) : QObject(parent)
{
    tcp_socket = new QTcpSocket();
}

void TCP_client::connectTo(QString ip_addr)
{


    connect(tcp_socket, SIGNAL(connected()), this, SLOT(onConnected()));
    connect(tcp_socket, SIGNAL(stateChanged(QAbstractSocket::SocketState)), this, SLOT(onStateChanged(QAbstractSocket::SocketState)));
    tcp_socket->connectToHost(QHostAddress(ip_addr), 1824);

    if (!tcp_socket->waitForConnected(5000))
    {
        qDebug() << "Error: " << tcp_socket->errorString();
    }


}

void TCP_client::onStateChanged(QAbstractSocket::SocketState state)
{

    switch (state)
    {
    case QAbstractSocket::UnconnectedState:
        emit TCPClientConnectionFailed();
        // qDebug()<<"[SOCKET STATE] Connecting Failed/Timeout";
        break;
    case QAbstractSocket::HostLookupState:
        // qDebug()<<"[SOCKET STATE] Host Lookup State";
        break;
    case QAbstractSocket::ConnectingState:
        // qDebug()<<"[SOCKET STATE] Connecting State";
        break;
    case QAbstractSocket::ConnectedState:
        // qDebug()<<"[SOCKET STATE] Connected State";
        break;
    case QAbstractSocket::BoundState:
        // qDebug()<<"[SOCKET STATE] Bound State";
        break;
    case QAbstractSocket::ListeningState:
        // qDebug()<<"[SOCKET STATE] Listening State";
        break;
    case QAbstractSocket::ClosingState:
        // qDebug()<<"[SOCKET STATE] Closing State";
        break;
    }
}



void TCP_client::onConnected()
{
    disconnect(tcp_socket, SIGNAL(connected()), this, SLOT(onConnected()));

    connect(tcp_socket, SIGNAL(disconnected()), this, SLOT(onDisconnected()));
    connect(tcp_socket, SIGNAL(readyRead()), this, SLOT(messageReady()));

    emit TCPClientConnected(tcp_socket->peerAddress().toString(), tcp_socket->peerPort());
}

void TCP_client::onDisconnected()
{
    disconnect(tcp_socket, SIGNAL(disconnected()));
    disconnect(tcp_socket, SIGNAL(readyRead()));
    disconnect(tcp_socket, SIGNAL(stateChanged(QAbstractSocket::SocketState)), this, SLOT(onStateChanged(QAbstractSocket::SocketState)));
    tcp_socket->close();
    emit TCPCLientDisconnected();
}

void TCP_client::sendMessage(QString string)
{
     QByteArray Data;
     Data.append(string);
     if (tcp_socket->state() == QTcpSocket::ConnectedState)
     {
         tcp_socket->write(Data);
         tcp_socket->flush();
     }
}


void TCP_client::disconnectCurrentConnection()
{
    tcp_socket->disconnectFromHost();
}

void TCP_client::closeClient()
{
    tcp_socket->close();
}
void TCP_client::abortConnection()
{
    tcp_socket->close();
}

void TCP_client::messageReady()
{
    /* Read TCP data */
    QByteArray received_buffer;
    received_buffer = tcp_socket->readAll();

    emit TCPClientNewMessage(tcp_socket->peerAddress().toString(), received_buffer);
//    /* Convert to JSON */
//    QString data = QString(received_buffer);
//    QJsonDocument tcp_json = QJsonDocument::fromJson(data.toUtf8());
//    QJsonObject tcp_received_data = tcp_json.object();

//    quint16 target_udp_port = quint16(tcp_received_data["openUdpPort"].toInt());

//    /* Emit */
//    if (target_udp_port != 2283) {
//        qWarning() << "Wrong Port: " << target_udp_port;
//    } else {
//        emit set_udp_port(target_udp_port);
//    }
}
