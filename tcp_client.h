#ifndef TCP_CLIENT_H
#define TCP_CLIENT_H

#include <QObject>
#include <QTcpSocket>
#include <QHostAddress>
#include <QDebug>
#include <QAbstractSocket>

class TCP_client : public QObject
{
    Q_OBJECT
public:
    explicit TCP_client(QObject *parent = nullptr);
    void connectTo(QString ip_addr);
    void sendMessage(QString string);
    void disconnectCurrentConnection();
    void closeClient();
    void abortConnection();

signals:
    void TCPClientNewMessage(const QString &from, const QString &message);
    void TCPClientConnected(const QString &from, quint16 port);
    void TCPCLientDisconnected();
    void TCPClientConnectionFailed();

public slots:
    void onConnected();
    void onDisconnected();
    void messageReady();
    void onStateChanged(QAbstractSocket::SocketState state);

private:
    QTcpSocket *tcp_socket;
    QByteArray array;
};

#endif // TCP_CLIENT_H
