#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(ui->open_udp_btn, &QPushButton::clicked, this, &MainWindow::connect_udp_target);
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

}
