#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    udpReceive  = new UdpReceiveForm(this);

    udpSend     = new UdpSendForm(this);

    ui->horizontalLayout->addWidget(udpReceive);

    ui->horizontalLayout->addWidget(udpSend);

    udpReceive->slotLoad();


    udpSend->slotLoad();

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    udpReceive->slotSave();
    udpSend->slotSave();
    event->accept();
}
