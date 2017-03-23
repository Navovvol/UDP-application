#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include "qudpextend.h"
namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    UdpReceiveForm* udpReceive;
    UdpSendForm*    udpSend;
protected:
    virtual void closeEvent(QCloseEvent *event);
};

#endif // MAINWINDOW_H
