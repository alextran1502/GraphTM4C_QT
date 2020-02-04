#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QComboBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    /* Scan for local ip address */
    MainWindow::scan_local_interface();

    /* Initialization of UDP Client */
    connect(ui->open_connection_btn, &QPushButton::clicked, this, &MainWindow::TCP_ping_device);

    if (udp_client == nullptr)
    {
        udp_client = new UDP_Client();
    }

    /* Initialization of TCP Client */
    if (tcp_client == nullptr)
    {
        tcp_client = new TCP_client();
    }



    ui->lcdNumber->setSegmentStyle(QLCDNumber::Flat);
    ui->lcdNumber->setMinimumHeight(48);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::scan_local_interface()
{
    QList<QNetworkInterface> interfaces = QNetworkInterface::allInterfaces();
    for (int i = 0; i < interfaces.count(); i++)
    {
        QList<QNetworkAddressEntry> entries = interfaces.at(i).addressEntries();
        for (int j = 0; j < entries.count(); j++)
        {
            if (entries.at(j).ip().protocol() == QAbstractSocket::IPv4Protocol)
            {
                ui->interface_combo_box->addItem(entries.at(j).ip().toString());
            }
        }
    }

}


void MainWindow::TCP_ping_device() 
{

    QString local_ip = ui->interface_combo_box->currentText();
    QString ip_address = ui->ip_address_input->text();
    QString port = ui->port_input->text();
    connect(tcp_client, &TCP_client::set_udp_port, this, &MainWindow::connect_udp_target);
    ui->statusbar->showMessage("Getting device info... ", 3000);

    tcp_client->open_socket(ip_address);
}

void MainWindow::connect_udp_target(quint16 port)
{
    QString local_ip = ui->interface_combo_box->currentText();
    ui->port_input->setText(QString::number(port));
    ui->statusbar->clearMessage();

    udp_client->bind_port(local_ip, port);
    connect(udp_client, &UDP_Client::udp_packet_available, this, &MainWindow::display_data);

    ui->port_input->setDisabled(true);
    disconnect(ui->open_connection_btn, &QPushButton::clicked, this, &MainWindow::TCP_ping_device);


    /* prepare button to close all connection */
    ui->open_connection_btn->setText("CLOSE");
    connect(ui->open_connection_btn, &QPushButton::clicked, this, [this] {
        this->tcp_client->closeSocketConnection();
        this->udp_client->closeUDP();
        disconnect(tcp_client, &TCP_client::set_udp_port, this, &MainWindow::connect_udp_target);

        ui->port_input->setDisabled(false);
        connect(ui->open_connection_btn, &QPushButton::clicked, this, &MainWindow::TCP_ping_device);

        ui->open_connection_btn->setText("OPEN");
    });
}

void MainWindow::display_data(const QString &message)
{
    QStringList result = message.split(",");
    double voltage = ((result.value(1).toDouble() / 32768) - 1) * 25;

    ui->lcdNumber->display(voltage);
}
