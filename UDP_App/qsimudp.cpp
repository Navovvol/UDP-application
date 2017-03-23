#include "qsimudp.h"

QSimUdp::QSimUdp(QObject *parent) : QObject(parent)
{
    m_timerReceive = new QTimer();
    connect(m_timerReceive,SIGNAL(timeout()),this,SLOT(slot_newPacket()));
    m_ms = timePacket;
    m_countPacket =0;
}

void QSimUdp::bind()
{
    m_timerReceive->start(m_ms);
    //m_countPacket =0;
}

void QSimUdp::abort()
{
    m_timerReceive->stop();
}

void QSimUdp::slot_newPacket()
{
    m_countPacket++;
    dataArray.clear();
    dataArray.append(quint8(111));
    dataArray.append(quint8(208));
    dataArray.append(m_countPacket);
    dataArray.append(quint8(1));
    dataArray.append(quint8(13));
    //memcpy(&m_countPacket,&Array[QuantityBytesAdd],sizeof(quint16));
    emit readyRead();
}
