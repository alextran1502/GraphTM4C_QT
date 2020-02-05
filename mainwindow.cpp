#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>
#include <QComboBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    initUI();

    /* Scan for local ip address */
    scanLocalInterface();

    /* Initialization of UDP Client */
    connect(ui->open_connection_btn, &QPushButton::clicked, this, &MainWindow::onTcpClientButtonClicked);

    if (udp_client == nullptr)
    {
        udp_client = new UDP_Client;
    }

    /* Initialization of TCP Client */
    if (tcp_client == nullptr)
    {
        tcp_client = new TCP_client;
    }


}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::initUI()
{

    ui->lcdNumber->setMinimumHeight(35);
    ui->statusbar->setStyleSheet("QStatusBar{padding-left:8px;background:rgba(214,214,214,125);"
                                 "color:rgba(127,127,127,255);"
                                 "font-weight: bold;"
                                 "font-size: 12px}");

    ui->plot->addGraph();
    ui->plot->graph(0)->setPen(QPen(QColor(40, 110, 255)));

    QSharedPointer<QCPAxisTickerTime> timeTicker(new QCPAxisTickerTime);
    timeTicker->setTimeFormat("%h:%m:%s");
    ui->plot->xAxis->setTicker(timeTicker);
    ui->plot->axisRect()->setupFullAxesBox();
    ui->plot->yAxis->setRange(-50, 50);
    connect(ui->plot->xAxis, SIGNAL(rangeChanged(QCPRange)), ui->plot->xAxis2, SLOT(setRange(QCPRange)));
    connect(ui->plot->yAxis, SIGNAL(rangeChanged(QCPRange)), ui->plot->yAxis2, SLOT(setRange(QCPRange)));

}

void MainWindow::scanLocalInterface()
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


/************************************************
 *
 * TCP CLIENT
 *
 ***********************************************/

void MainWindow::onTcpClientButtonClicked()
{
    disconnect(ui->open_connection_btn, &QPushButton::clicked, this, &MainWindow::onTcpClientButtonClicked);

    /* Change UI */
    ui->open_connection_btn->setText("CLOSE");  // TODO add logic to handle close port
    ui->statusbar->showMessage("Opening TCP Port... ");

    /* Logic */
    QString ip_address = ui->ip_address_input->text();


    connect(tcp_client, &TCP_client::TCPClientConnected, this, &MainWindow::onTcpClientNewConnection);
    connect(tcp_client, &TCP_client::TCPClientConnectionFailed, this, &MainWindow::onTcpClientTimeout);
    tcp_client->connectTo(ip_address);

}


/**
 * @brief MainWindow::onTcpClientNewConnection
 * @param from
 * @param port
 */
void MainWindow::onTcpClientNewConnection(const QString &from, quint16 port)
{
    ui->statusbar->showMessage("Established Connection To " + from + ":" + QString::number(port), 5000);

    // Connect to a slot to listen to a response
    connect(tcp_client, &TCP_client::TCPClientNewMessage, this, &MainWindow::onTcpClientNewMessage);
    connect(ui->open_connection_btn, SIGNAL(clicked()), this, SLOT(onTcpClientDisconnectButtonClicked()));
    // Sending a ping message to Tiva
    tcp_client->sendMessage("Ping");
}


void MainWindow::onTcpClientDisconnectButtonClicked()
{
    disconnect(ui->open_connection_btn, SIGNAL(clicked()), this, SLOT(onTcpClientDisconnectButtonClicked()));
    tcp_client->abortConnection();
    udp_client->closeUDP();
}


void MainWindow::onTcpClientNewMessage(const QString &from, const QString &message)
{
    Q_UNUSED(from)

    disconnect(tcp_client, &TCP_client::TCPClientNewMessage, this, &MainWindow::onTcpClientNewMessage);

    // Handle Message
    handleTcpPacket(message);
}


void MainWindow::onTcpClientTimeout()
{
    disconnect(tcp_client, &TCP_client::TCPClientConnected, this, &MainWindow::onTcpClientNewConnection);
    disconnect(tcp_client, &TCP_client::TCPClientConnectionFailed, this, &MainWindow::onTcpClientTimeout);
    disconnect(ui->open_connection_btn, &QPushButton::clicked, this, &MainWindow::onTcpClientDisconnectButtonClicked);

    /* Hanndle UI */
    ui->statusbar->showMessage("Connection Timeout", 3000);
    ui->open_connection_btn->setText("OPEN");


    /* Logic */
    tcp_client->closeClient();
    connect(ui->open_connection_btn, &QPushButton::clicked, this, &MainWindow::onTcpClientButtonClicked);

}


/**
 * @brief MainWindow::onUdpConnected
 * @param port target UDP port
 */
void MainWindow::onUdpConnected(quint16 port)
{
    QString local_ip = ui->interface_combo_box->currentText();
    ui->port_input->setText(QString::number(port));
    ui->statusbar->clearMessage();

    udp_client->bind_port(local_ip, port);
    connect(udp_client, &UDP_Client::udp_packet_available, this, &MainWindow::display_data);

}

/**************************************************
 *
 * MainWindow Logic
 *
 *************************************************/
void MainWindow::handleTcpPacket(const QString &TcpPacket)
{
        QString local_ip = ui->interface_combo_box->currentText();
        qDebug() << "Handled: " << TcpPacket;
        /* Convert to JSON */
        QString data = TcpPacket;
        QJsonDocument tcp_json = QJsonDocument::fromJson(data.toUtf8());
        QJsonObject tcp_received_data = tcp_json.object();

        quint16 target_udp_port = quint16(tcp_received_data["openUdpPort"].toInt());

        /* Emit */
        if (target_udp_port != 2283) {
            qWarning() << "Wrong Port: " << target_udp_port;
        } else {
            qDebug() << "Port" << target_udp_port;

            connect(udp_client, &UDP_Client::udp_packet_available, this, &MainWindow::display_data);
            udp_client->bind_port(local_ip, target_udp_port);

        }
}

/***************************************************
 *
 * GRAPHING
 *
 ***************************************************/
void MainWindow::display_data(const QString &message)
{
    QStringList result = message.split(",");
    double voltage = ((result.value(1).toDouble() / 8388608) - 1) * 25;
    double accleration = voltage / 0.0052;
    ui->lcdNumber->display(accleration);

    // Plot
    static QTime time(QTime::currentTime());
    double key = time.elapsed()/1000.0;

    ui->plot->graph(0)->addData(key, accleration);

    ui->plot->yAxis->setRange(-50, 50);
    ui->plot->graph(0)->rescaleValueAxis(false, true);
//    double minrangeLine = *std::min_element(linenb.begin(),linenb.end());
//    double maxrangeLine = *std::max_element(linenb.begin(),linenb.end());
//    ui->plot->axisRect(0)->axis(QCPAxis::atLeft)->rescale();


    ui->plot->xAxis->setRange(key, 8, Qt::AlignRight);
    ui->plot->replot();



}
