#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "udp_client.h"
#include "tcp_client.h"
#include <QNetworkInterface>

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
    void scan_local_interface();

public slots:
    void connect_udp_target();
private:
    Ui::MainWindow *ui;
    UDP_Client* udp_client = nullptr;
    TCP_client* tcp_client = nullptr;
};
#endif // MAINWINDOW_H
