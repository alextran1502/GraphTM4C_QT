#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(ui->open_udp_btn, &QPushButton::clicked, this, &MainWindow::connect_udp_target);

    if (udp_client == nullptr)
    {
        udp_client = new UDP_Client();
    }

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::connect_udp_target()
{
    QString ip_address = ui->ip_address_input->text();
    QString port = ui->port_input->text();


    qDebug() << "Connect" << ip_address << "-" << port.toShort();
    udp_client->bind_port(ip_address, quint16(port.toShort()));
    connect(udp_client, &UDP_Client::udp_packet_available, this, &MainWindow::display_data);
}

void MainWindow::display_data(const QString &message)
{
//    qDebug() << "got some" << message;

    ui->lcdNumber->display(message);
}
