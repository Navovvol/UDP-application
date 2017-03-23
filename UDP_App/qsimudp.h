#ifndef QSIMUDP_H
#define QSIMUDP_H

#include <QObject>
#include <QTimer>
const int timePacket = 20;
const int sizePacket = 6;
const int indexPos = 2;
class QSimUdp : public QObject
{
    Q_OBJECT
public:
    explicit QSimUdp(QObject *parent = 0);
    QByteArray dataArray;// массив данных
signals:
    readyRead();
public slots:
    void bind();//открыть порт на прием
    void abort();//остановить прием
private slots:
    void slot_newPacket();// create new packet
private:
    QTimer* m_timerReceive;//по таймеру получаем пакеты
    int m_ms;//
    quint16 m_countPacket;// счетчик пакетов
};

#endif // QSIMUDP_H
