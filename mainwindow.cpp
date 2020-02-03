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
    connect(ui->open_udp_btn, &QPushButton::clicked, this, &MainWindow::connect_udp_target);

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
//        qDebug() << "Interface: " << interfaces.at(i).humanReadableName();
        QList<QNetworkAddressEntry> entries = interfaces.at(i).addressEntries();
        for (int j = 0; j < entries.count(); j++)
        {
            if (entries.at(j).ip().protocol() == QAbstractSocket::IPv4Protocol)
            {

                ui->interface_combo_box->addItem(QString("[%1]: %2")
                                                 .arg(interfaces.at(i).humanReadableName())
                                                 .arg(entries.at(j).ip().toString())
                                                 );
            }
        }
    }

}

void MainWindow::connect_udp_target()
{
    QString ip_address = ui->ip_address_input->text();
    QString port = ui->port_input->text();
    QString local_ip = ui->interface_combo_box->currentText();

    qDebug() << "Local IP for TCP: " << local_ip;

    qDebug() << "Connect" << ip_address << "-" << port.toShort();
//    udp_client->bind_port(ip_address, quint16(port.toShort()));
//    connect(udp_client, &UDP_Client::udp_packet_available, this, &MainWindow::display_data);
}

void MainWindow::display_data(const QString &message)
{
    QStringList result = message.split(",");
    double voltage = ((result.value(1).toDouble() / 32768) - 1) * 25;

    ui->lcdNumber->display(voltage);
}
