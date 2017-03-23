#ifndef QUDPEXTEND_H
#define QUDPEXTEND_H

#include <QObject>
#include <QTime>
#include <QTimer>
#include <QStandardItem>
#include <QStandardItemModel>
#include <QString>
#include <QUdpSocket>
#include <QTableView>
#include <QPainter>
#include <QComboBox>
#include <QItemDelegate>
#include <QMenu>
#include <QAction>
#include <QFile>
#include <QMessageBox>
#include <qmath.h>

#include "qsimudp.h"

// регулярные выражения. нужны для блокировки ввода ненужных символов http://www.regexlib.com/Search.aspx
#include <QRegExpValidator>
#define IP "^(\\d{1,2}|1\\d\\d|2[0-4]\\d|25[0-5])\\.(\\d{1,2}|1\\d\\d|2[0-4]\\d|25[0-5])\\.(\\d{1,2}|1\\d\\d|2[0-4]\\d|25[0-5])\\.(\\d{1,2}|1\\d\\d|2[0-4]\\d|25[0-5])$"

const QString s_uint8   = "1";
const QString s_int8    = "4";
const QString s_uint16  = "2";
const QString s_int16   = "5";
const QString s_uint32  = "3";
const QString s_int32   = "6";
const QString symb_uint8   = "uint8";
const QString symb_int8    = "int8";
const QString symb_uint16  = "uint16";
const QString symb_int16   = "int16";
const QString symb_uint32  = "uint32";
const QString symb_int32   = "int32";
const QString s_HEX = "16";
const QString s_DEC = "10";
const QString s_BIN = "2";
const QString symb_HEX = "HEX";
const QString symb_DEC = "DEC";
const QString symb_BIN = "BIN";
const QString strHeaderBytes = "Байты";
const QString strHeaderType = "     Тип     ";
const QString strHeaderView = "     Вид     ";
const QString strHeaderData = "                        Данные                          ";

#include <QNetworkInterface>


class Combo_Delegate : public QItemDelegate
{
    Q_OBJECT
    QHash<int, QString> m_values;

public:
    Combo_Delegate(QObject * parent = 0);

    QWidget * createEditor( QWidget * parent,
                                const QStyleOptionViewItem &option,
                                const QModelIndex &index ) const;
        void setEditorData( QWidget * editor, const QModelIndex &index ) const;
        void setModelData( QWidget *editor, QAbstractItemModel *model,
                           const QModelIndex &index ) const;
        void updateEditorGeometry( QWidget *editor,
                                   const QStyleOptionViewItem &option,
                                   const QModelIndex &index) const;
        void paint( QPainter * painter, const QStyleOptionViewItem &option,
                    const QModelIndex &index ) const;

        inline QHash<int, QString>& values() { return m_values; }

        QString curText;
private slots:
        void slotChanged(QString);
signals:
        void ChangeIndex();
};

class QudpExtend : public QObject
{
    Q_OBJECT

    Q_PROPERTY(bool csvRec READ csvRec WRITE setCsvRec NOTIFY csvRecChanged)
    Q_PROPERTY(bool convertRec READ convertRec WRITE setConvertRec NOTIFY convertRecChanged)
    Q_PROPERTY(UdpMode udpMode READ udpMode WRITE setUdpMode NOTIFY udpModeChanged)
    Q_PROPERTY(int numPort READ numPort WRITE setNumPort NOTIFY numPortChanged)//номер порта на прием и передачу
    Q_PROPERTY(int ms_send READ ms_send WRITE setMs_send NOTIFY ms_sendChanged)
    Q_PROPERTY(QString hostAddress READ hostAddress WRITE setHostAddress NOTIFY hostAddressChanged)//Адрес на передачу

public:
    explicit QudpExtend(QObject *parent = 0);

    bool convertRec()const;


    bool csvRec()const;


    enum UdpMode{OFF,RECEIVE,SEND};
    Q_ENUM(UdpMode)
    UdpMode udpMode()const;
    void setUdpMode(const UdpMode &udpMode);

    int numPort()const;


    int ms_send()const;


    QString hostAddress()const;


    void setModelToTableView(QTableView *tableview);

signals:
    //recordDataChanged(RecordData);
    convertRecChanged(bool);
    csvRecChanged(bool);
    udpModeChanged(UdpMode);
    numPortChanged(int);
    ms_sendChanged(int);
    signal_MessageChanged(QString);
    hostAddressChanged(QString);
    signal_ColorBTNChange(QString);//сигнал изменения цвета кнопки включить прием
    signal_QuantByte(QString);//сигнал кол-во полученный байт
    signal_TimePacket(QString);//сигнал время между пакетами
    signal_EnableChanged(bool);// сигнал изменение активности записи входных данных в лог
public slots:
    void setConvertRec(const bool &cnvRec);
    void setCsvRec(const bool &t);
    void setNumPort(const int &nport);
    void setMs_send(const int &ms);
    void setHostAddress(const QString &host);

    void slot_timer_paint(); // отрисовка пакетов и графиков
    void slot_WatchDog(); // вачдог для обрыв связи при приеме данных
    void slot_timer_1sec(); // слот количества пакетов в сек.
    void slot_timer_Send(); // слот таймера на передачу данных
    void slot_numSysChangedReceiv();
    void slotCustomMenuRequested_R(QPoint pos);
    void slotTypeAll();
    void slotViewAll();
    void slotAddRow();// добавить строку
    void slotRecordOn(); // включить запись лога входящий байт
    void slotOpenRecord();// открыть лог

    void slotSaveXML(const QString &filename);
    void slotLoadXML(const QString &filename);

private slots:

    void processPendingDatagrams();
    void remapByte();
    void deleteRowModel();
protected:
    void AddRow(int *QuantityBytes,QString sBytes,QString sType,QString sView,QString Data);
    int HexToInt_symb(QChar ch_symb);
    int HexToInt(QString str_hex, int bytes);
    void strHexNeg(QString *str);
    QString Convert_receive_Byte(int i, quint8 *Array); // Конвертировать Принятый байт по таблице
    QString Convert_receive_Byte_Extend(QString strRet,QString strType,int isize);
    void remap(QString ComboText, int *quantity,int row);
    void fncNumSysChange(Combo_Delegate *comboDelegate,int n_row);

    template <typename T> void transNumSys(T &num, QString newText, int row);
    template <typename T> bool strToArray(T &num, int i, quint8 *Array, int N_Bytes);
private:
    void ReceiveOn();
    void ReceiveOff();
    void SendOn();
    void SendOff();
    void SetMessage(const QString &message);
    QUdpSocket *udpSocket;

    int     QuantityBytesIn; // количество байт в пакете на приеме данных
    int     QuantityBytesAdd; // количество байт настроенных в таблице
    int     QuantityBytesOut; // количество байт передано
    int     QuantityPacketsIn;// количество входящих пакетов в секунду.
    bool    sendOK; // Передача работает
    int     delay_in;
    QTime*  timer; // вычисление времени между пакетами (receive)
    QTimer* timer_Send;//таймер на передачу пакетов
    QTimer* timer_1sec;// таймер 1 сек. подсчет количества передаваемых пакетов в секунду
    int     m_ms_send;//время между пакетами на передачу.
    QTimer* timer_paint; //таймер на обновление данных в таблице
    QTimer* WathDog_Receive; // вачдог на прием данных
    QStandardItem *item;
    QStandardItemModel *model;
    QByteArray datagram;
    QString str_datagram;// data in string

    QTableView *tableview;

    UdpMode     m_udpMode;// режим UDP / прием передача
    bool        m_csvRec;//запись в csv файл
    bool        m_convertRec;// конвертировать данные лога по таблице
    int         m_numPort;// Номер порта
    QString     m_message;//сообщение
    QString     m_hostAddress;// IP адрес носта на передачу

    //--------------------------------------------------------------------------
    bool        Record_On;      // Запись включена
    QFile       *File_Record;   // файл для записи
    QTextStream *outstream;     // поток для записи

    bool butBlink;//Мигание кнопки Включить прием
    Combo_Delegate * pCombo_DelegateReceiv;
    Combo_Delegate * tCombo_DelegateReceiv; //
    int countTable;
    //---------------------------------------
    QSimUdp *simulateUDP;
    const bool simOn=0;
};
//-----------------------------------------------
#include <QtWidgets>
#include <QtUiTools>
//===============================================
class UdpReceiveForm: public QWidget {
Q_OBJECT
private:
    QudpExtend*     m_UDP;
    QTableView*     m_tableView;
    QPushButton*    m_btn_Add;
    QPushButton*    m_btn_Delete;
    QPushButton*    m_btn_ReceiveOn;// прием включить
    QPushButton*    m_btn_RecordOn;// запись лога включить
    QPushButton*    m_btn_OpenRecord;// Открыть запись лога
    QLabel*         m_lblMessage;
    QLabel*         m_lblQuant;
    QLabel*         m_lblTime;
    QSpinBox*       m_spB_Port;
    QRadioButton*   m_rb_txt;
    QRadioButton*   m_rb_csv;
    QRadioButton*   m_rb_convert;
    QRadioButton*   m_rb_native;
public:
    UdpReceiveForm(QWidget* pwgt=0) : QWidget(pwgt)
    {
        QUiLoader*  puil = new QUiLoader(this);
        QFile       file(":/form.ui");
        m_UDP = new QudpExtend(this);

        QWidget* pwgtForm = puil->load(&file);
        if(pwgtForm){
            resize(pwgtForm->size());

            m_tableView = pwgtForm->findChild<QTableView*>("tbl_Data");
            m_UDP->setModelToTableView(m_tableView);

            m_btn_Add       = pwgtForm->findChild<QPushButton*>("btn_Add");
            m_btn_Delete    = pwgtForm->findChild<QPushButton*>("btn_Delete");
            m_btn_ReceiveOn = pwgtForm->findChild<QPushButton*>("btn_ReceiveOn");
            m_btn_RecordOn  = pwgtForm->findChild<QPushButton*>("btn_RecordOn");
            m_btn_OpenRecord= pwgtForm->findChild<QPushButton*>("btn_OpenRecord");
            m_lblMessage    = pwgtForm->findChild<QLabel*>("lbl_Message");
            m_lblQuant      = pwgtForm->findChild<QLabel*>("label_Quant");
            m_lblTime       = pwgtForm->findChild<QLabel*>("label_Time");
            m_spB_Port      = pwgtForm->findChild<QSpinBox*>("spinBox_Port");
            m_rb_txt        = pwgtForm->findChild<QRadioButton*>("radioButton_txt");
            m_rb_csv        = pwgtForm->findChild<QRadioButton*>("radioButton_csv");
            m_rb_convert    = pwgtForm->findChild<QRadioButton*>("radioButton_Convert");
            m_rb_native     = pwgtForm->findChild<QRadioButton*>("radioButton_Native");


            if(m_UDP->csvRec())m_rb_csv->setChecked(1);
            else m_rb_txt->setChecked(1);
            if(m_UDP->convertRec())m_rb_convert->setChecked(1);
            else m_rb_native->setChecked(1);



            m_spB_Port->setValue(m_UDP->numPort());

            m_lblMessage->setText("");


            m_btn_Add->setIcon(QIcon(":/icons/edit_add.ico"));
            m_btn_Delete->setIcon(QIcon(":/icons/edit_remove.ico"));

            connect(m_btn_Add,SIGNAL(clicked(bool)),m_UDP,SLOT(slotAddRow()));
            connect(m_btn_Delete,SIGNAL(clicked(bool)),m_UDP,SLOT(deleteRowModel()));            
            connect(m_btn_ReceiveOn,SIGNAL(clicked(bool)),this,SLOT(slotReceiveClicked()));
            connect(m_btn_RecordOn,SIGNAL(clicked(bool)),m_UDP,SLOT(slotRecordOn()));
            connect(m_btn_OpenRecord,SIGNAL(clicked(bool)),m_UDP,SLOT(slotOpenRecord()));
            connect(m_rb_csv,SIGNAL(toggled(bool)),m_UDP,SLOT(setCsvRec(bool)));
            connect(m_rb_convert,SIGNAL(toggled(bool)),m_UDP,SLOT(setConvertRec(bool)));

            connect(m_UDP,SIGNAL(signal_MessageChanged(QString)),m_lblMessage,SLOT(setText(QString)));
            connect(m_UDP,SIGNAL(signal_ColorBTNChange(QString)),m_btn_ReceiveOn,SLOT(setStyleSheet(QString)));
            connect(m_UDP,SIGNAL(signal_QuantByte(QString)),m_lblQuant,SLOT(setText(QString)));
            connect(m_UDP,SIGNAL(signal_TimePacket(QString)),m_lblTime,SLOT(setText(QString)));
            connect(m_UDP,SIGNAL(signal_EnableChanged(bool)),this,SLOT(slot_Enabled(bool)));
            connect(m_UDP,SIGNAL(numPortChanged(int)),m_spB_Port,SLOT(setValue(int)));

            //Layout setup
            QHBoxLayout* phbxLayout = new QHBoxLayout;
            phbxLayout->addWidget(pwgtForm);
            setLayout(phbxLayout);
        }
    }
public slots:
    void slotReceiveClicked(){
        if(m_UDP->udpMode()!=QudpExtend::UdpMode::RECEIVE){
            if(m_spB_Port->text()!=""){
                m_UDP->setNumPort(m_spB_Port->value());
            }
            m_UDP->setUdpMode(QudpExtend::UdpMode::RECEIVE);
        }
        else{
            m_UDP->setUdpMode(QudpExtend::UdpMode::OFF);
        }
    }
    void slot_Enabled(const bool &t){
        m_rb_txt->setEnabled(t);
        m_rb_csv->setEnabled(t);
        m_rb_convert->setEnabled(t);
        m_rb_native->setEnabled(t);
        m_btn_OpenRecord->setEnabled(t);
    }
    void slotSave(){
        m_UDP->slotSaveXML(QDir::currentPath()+QDir::separator()+"receive_settings.xml");
    }
    void slotLoad(){
        m_UDP->slotLoadXML(QDir::currentPath()+QDir::separator()+"receive_settings.xml");
    }
};
class UdpSendForm: public QWidget{
Q_OBJECT
private:
    QudpExtend*     m_UDP;
    QTableView*     m_tableView;
    QPushButton*    m_btn_Add;
    QPushButton*    m_btn_Delete;
    QPushButton*    m_btn_SendOn;// передачу включить
    QLabel*         m_lblMessage;
    QLabel*         m_lblQuant;
    QSpinBox*       m_spB_Time;
    QSpinBox*       m_spB_Port;
    QLineEdit*      m_lEdit_IP;
public:
    UdpSendForm(QWidget* pwgt=0) : QWidget(pwgt)
    {
        QUiLoader*  puil = new QUiLoader(this);
        QFile       file(":/formSend.ui");
        m_UDP = new QudpExtend(this);

        QWidget* pwgtForm = puil->load(&file);
        if(pwgtForm){
            resize(pwgtForm->size());

            m_tableView = pwgtForm->findChild<QTableView*>("tbl_Data");
            m_UDP->setModelToTableView(m_tableView);

            m_btn_Add       = pwgtForm->findChild<QPushButton*>("btn_Add");
            m_btn_Delete    = pwgtForm->findChild<QPushButton*>("btn_Delete");
            m_btn_SendOn    = pwgtForm->findChild<QPushButton*>("btn_SendOn");
            m_lblMessage    = pwgtForm->findChild<QLabel*>("lbl_Message");
            m_lblQuant      = pwgtForm->findChild<QLabel*>("label_Quant");
            m_spB_Time      = pwgtForm->findChild<QSpinBox*>("spinBox_Time");
            m_spB_Port      = pwgtForm->findChild<QSpinBox*>("spinBox_Port");
            m_lEdit_IP      = pwgtForm->findChild<QLineEdit*>("lineEdit_IP_Send");

            m_spB_Port->setValue(m_UDP->numPort());

            m_spB_Time->setValue(m_UDP->ms_send());

            m_lblMessage->setText("");

            QRegExp regExp(IP);
            QRegExpValidator *RegExpValidator_IP = new QRegExpValidator(regExp,this);
            m_lEdit_IP->setValidator(RegExpValidator_IP);
            m_lEdit_IP->setInputMask("000.000.000.000;_");
            m_lEdit_IP->setText(m_UDP->hostAddress());

            m_btn_Add->setIcon(QIcon(":/icons/edit_add.ico"));
            m_btn_Delete->setIcon(QIcon(":/icons/edit_remove.ico"));

            connect(m_btn_Add,SIGNAL(clicked(bool)),m_UDP,SLOT(slotAddRow()));
            connect(m_btn_Delete,SIGNAL(clicked(bool)),m_UDP,SLOT(deleteRowModel()));
            connect(m_btn_SendOn,SIGNAL(clicked(bool)),this,SLOT(slotSendClicked()));
              /*numPortChanged(int);
                ms_sendChanged(int);*/
            connect(m_UDP,SIGNAL(signal_MessageChanged(QString)),m_lblMessage,SLOT(setText(QString)));
            connect(m_UDP,SIGNAL(signal_ColorBTNChange(QString)),m_btn_SendOn,SLOT(setStyleSheet(QString)));
            connect(m_UDP,SIGNAL(signal_QuantByte(QString)),m_lblQuant,SLOT(setText(QString)));
            connect(m_UDP,SIGNAL(hostAddressChanged(QString)),m_lEdit_IP,SLOT(setText(QString)));
            connect(m_UDP,SIGNAL(numPortChanged(int)),m_spB_Port,SLOT(setValue(int)));
            connect(m_UDP,SIGNAL(ms_sendChanged(int)),m_spB_Time,SLOT(setValue(int)));
            //connect(m_UDP,SIGNAL(signal_EnableChanged(bool)),this,SLOT(slot_Enabled(bool)));

            //Layout setup
            QHBoxLayout* phbxLayout = new QHBoxLayout;
            phbxLayout->addWidget(pwgtForm);
            setLayout(phbxLayout);
        }
    }
public slots:
    void slotSendClicked(){
        if(m_UDP->udpMode()!=QudpExtend::UdpMode::SEND){
            if(m_spB_Port->text()!=""){
                m_UDP->setNumPort(m_spB_Port->value());
            }
            if(m_lEdit_IP->text()!=""){
                m_UDP->setHostAddress(m_lEdit_IP->text());
            }
            if(m_spB_Time->text()!="")
            {
                m_UDP->setMs_send(m_spB_Time->value());
            }
            m_UDP->setUdpMode(QudpExtend::UdpMode::SEND);
        }
        else{
            m_UDP->setUdpMode(QudpExtend::UdpMode::OFF);
        }
    }

    void slotSave(){
        m_UDP->slotSaveXML(QDir::currentPath()+QDir::separator()+"send_settings.xml");
    }
    void slotLoad(){
        m_UDP->slotLoadXML(QDir::currentPath()+QDir::separator()+"send_settings.xml");
    }
};

#endif // QUDPEXTEND_H
