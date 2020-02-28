#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "udp_client.h"
#include "tcp_client.h"
#include <QNetworkInterface>
#include <QJsonDocument>
#include <QJsonObject>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void display_data(const QString &message);
    void scanLocalInterface();
    void handleTcpPacket(const QString &TcpPacket);
    
public slots:
    void onUdpConnected(quint16 port);
    void onTcpClientButtonClicked();
    void onTcpClientDisconnectButtonClicked();
    void onTcpClientNewConnection(const QString &from, quint16 port);
    void onTcpClientTimeout();
    void onTcpClientNewMessage(const QString &from, const QString &message);

private:
    Ui::MainWindow *ui;
    void initUI();
    quint64 dataPoint = 0;
    UDP_Client* udp_client = nullptr;
    TCP_client* tcp_client = nullptr;
};
#endif // MAINWINDOW_H
