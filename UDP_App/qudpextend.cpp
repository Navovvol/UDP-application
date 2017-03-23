#include "qudpextend.h"
const int ms_paint = 300; // время таймера timer_paint
const int wachdog_delay = 250;// время вачдога

Combo_Delegate::Combo_Delegate(QObject *parent) : QItemDelegate(parent)
{

}

QWidget * Combo_Delegate::createEditor( QWidget * parent,
                                      const QStyleOptionViewItem& /* option */,
                                      const QModelIndex& /* index */) const
{
    QComboBox * pEditor = new QComboBox( parent );

    QHash<int, QString>::const_iterator i = m_values.constBegin();
    while ( i != m_values.constEnd() )
    {
        pEditor -> addItem( i.value(), i.key() );
        ++i;
     }

    pEditor -> installEventFilter( const_cast<Combo_Delegate*>( this ) );
    connect(pEditor,SIGNAL(currentTextChanged(QString)),this,SLOT(slotChanged(QString)));
    return pEditor;
}

void Combo_Delegate::setEditorData( QWidget * editor, const QModelIndex &index) const
{
    int value = index.model()->data( index, Qt::EditRole ).toInt();
    QComboBox * cb = static_cast<QComboBox*>( editor );

    int idx = cb->findData( value );
    if ( idx < 0 ) return;
    cb->setCurrentIndex( idx );
}

void Combo_Delegate::setModelData( QWidget * editor,
                                    QAbstractItemModel *model,
                                    const QModelIndex& index) const
{
    QComboBox * cb = static_cast<QComboBox*>( editor );
    int idx = cb->currentIndex();
    if ( idx < 0 ) return;
    int value = cb->itemData( idx ).toInt();
    model->setData( index, value );
}

void Combo_Delegate::updateEditorGeometry( QWidget *editor,
                                         const QStyleOptionViewItem &option,
                                         const QModelIndex& /* index */) const
{
    editor->setGeometry( option.rect );
}

void Combo_Delegate::paint( QPainter * painter, const QStyleOptionViewItem &option,
                          const QModelIndex &index ) const
{
    QStyleOptionViewItem opt = option;
    int value = index.data().toInt();


    QString s = m_values[ value ];
    QVariant color = index.data( Qt::TextColorRole );

    if ( color.isValid() && qvariant_cast<QColor>(color).isValid() )
        opt.palette.setColor( QPalette::Text, qvariant_cast<QColor>(color) );

    opt.displayAlignment = Qt::AlignVCenter | Qt::AlignLeft;

    drawDisplay(painter, opt, opt.rect, s);
    drawFocus(painter, opt, opt.rect);
}
void Combo_Delegate::slotChanged(QString index)
{
    curText = index;
    emit ChangeIndex();
}
//-------------------------------------------------------------
//-------------------------------------------------------------
//-------------------------------------------------------------
//-------------------------------------------------------------
//-------------------------------------------------------------
//-------------------------------------------------------------
int QudpExtend::HexToInt(QString str_hex,int bytes)
{
    int res=0;
    int degree=0;
    int max_symb = bytes * 2;//размерность * 2, получаем полное кол-во символов, т.к. один байт = 2 символа (FF)
    int firts_symb = HexToInt_symb(str_hex.at(0)); // перевод первого символа в число
    bool negativ_num = 0;// флаг - отрицательное число
    //Является ли число отрицательным
    if((str_hex.size()>=max_symb)&(firts_symb>=8))
    {
        strHexNeg(&str_hex);
        negativ_num = 1;
    }
    for(int i=str_hex.size()-1;i>=0;i--)
    {
        res += HexToInt_symb(str_hex.at(i)) * qPow(16,degree);
        degree++;
    }
    if(negativ_num)
    {
        res++;
        res = - res;
    }
    return(res);
}
void QudpExtend::strHexNeg(QString *str)
{
    /*Строку HEX делает из отрицательного числа положительное, меньшее на 1 */
    QString neg ="";
    QString symb ="";
    for(int i=0;i<str->size();i++)
    {
        QString temp(str->at(i));
        if(temp=="0")symb="F";
        else if(temp=="0")symb="F";
        else if(temp=="1")symb="E";
        else if(temp=="2")symb="D";
        else if(temp=="3")symb="C";
        else if(temp=="4")symb="B";
        else if(temp=="5")symb="A";
        else if(temp=="6")symb="9";
        else if(temp=="7")symb="8";
        else if(temp=="8")symb="7";
        else if(temp=="9")symb="6";
        else if(temp=="A")symb="5";
        else if(temp=="B")symb="4";
        else if(temp=="C")symb="3";
        else if(temp=="D")symb="2";
        else if(temp=="E")symb="1";
        else if(temp=="F")symb="0";
        neg.append(symb);
    }
    *str = neg;
}
int QudpExtend::HexToInt_symb(QChar ch_symb)
{
    QString symb(ch_symb);
    int num = 0;
    if(symb == "A")num = 10;
    else if(symb == "B")num = 11;
    else if(symb == "C")num = 12;
    else if(symb == "D")num = 13;
    else if(symb == "E")num = 14;
    else if(symb == "F")num = 15;
    else if(symb == "9")num = 9;
    else if(symb == "8")num = 8;
    else if(symb == "7")num = 7;
    else if(symb == "6")num = 6;
    else if(symb == "5")num = 5;
    else if(symb == "4")num = 4;
    else if(symb == "3")num = 3;
    else if(symb == "2")num = 2;
    else if(symb == "1")num = 1;
    else if(symb == "0")num = 0;
    return(num);
}
//-------------------------------------------------------------
//-------------------------------------------------------------
//-------------------------------------------------------------
//-------------------------------------------------------------
//-------------------------------------------------------------
//-------------------------------------------------------------
QudpExtend::QudpExtend(QObject *parent) : QObject(parent)
{
    //Настройка UDP сокета на прием
    //---------------------------------------------------------
    udpSocket = new QUdpSocket(this);
    /*--------*/
    simulateUDP = new QSimUdp(this);

    if(!simOn) connect(udpSocket,SIGNAL(readyRead()),this,SLOT(processPendingDatagrams()));
    else if(simOn){
        connect (simulateUDP,SIGNAL(readyRead()),this,SLOT(processPendingDatagrams()));
        qDebug()<<"включен режим симуляции Приема Пакетов";
    }
    /*---------*/
    QuantityBytesIn = 0;//Счетчик байтов// прием
    QuantityBytesAdd = 0;//Счетчик байтов для таблица приема
    QuantityBytesOut=0; // байт передано в пакете
    sendOK = false;

    str_datagram.clear();

    m_convertRec = false;
    m_csvRec = false;
    m_udpMode = UdpMode::OFF;
    m_numPort=0;
    m_ms_send=5;
    m_hostAddress = "192.168.0.3";
    m_message="no message";
    Record_On= false;
    //-----------------------------------------------------------
    //Таймеры
    timer = new QTime();// Таймер для вычисления времени между пакетами / Прием

    timer_Send = new QTimer();//Таймер на передачу пакетов

    timer_paint = new QTimer();//Таймер для вывода принятых данных в таблицу // мигание кнопки
    connect(timer_paint,SIGNAL(timeout()),this,SLOT(slot_timer_paint()));
    connect(timer_Send,SIGNAL(timeout()),this,SLOT(slot_timer_Send()));

    WathDog_Receive = new QTimer();
    connect(WathDog_Receive,SIGNAL(timeout()),this,SLOT(slot_WatchDog()));

    timer_1sec = new QTimer();
    connect(timer_1sec,SIGNAL(timeout()),this,SLOT(slot_timer_1sec()));
    QuantityPacketsIn = 0;
    model = new QStandardItemModel;

    butBlink=0;
    countTable = 0;
}

bool QudpExtend::convertRec() const
{
    return m_convertRec;
}

void QudpExtend::setConvertRec(const bool &cnvRec)
{
    if(m_convertRec!=cnvRec)
    {
        m_convertRec=cnvRec;
        emit convertRecChanged(m_convertRec);
        //qDebug()<<"setConvertRec="<<m_convertRec;
    }
}

bool QudpExtend::csvRec() const
{
    return m_csvRec;
}

void QudpExtend::setCsvRec(const bool &t)
{
    if(m_csvRec!=t)
    {
        m_csvRec=t;
        emit csvRecChanged(m_csvRec);
        //qDebug()<<"setCsvRec="<<m_csvRec;
    }
}

QudpExtend::UdpMode QudpExtend::udpMode() const
{
    return m_udpMode;
}

void QudpExtend::setUdpMode(const QudpExtend::UdpMode &udpMode)
{
    if(m_udpMode!=udpMode)
    {
        if((m_udpMode==UdpMode::OFF)&&(udpMode==UdpMode::RECEIVE)) ReceiveOn();
        else if((m_udpMode==UdpMode::RECEIVE)&&(udpMode==UdpMode::OFF)) ReceiveOff();
        else if((m_udpMode==UdpMode::OFF)&&(udpMode==UdpMode::SEND))SendOn();
        else if((m_udpMode==UdpMode::SEND)&&(udpMode==UdpMode::OFF))SendOff();
        else if((m_udpMode==UdpMode::RECEIVE)&&(udpMode==UdpMode::SEND))
        {
            ReceiveOff();
            SendOn();
        }
        else if((m_udpMode==UdpMode::SEND)&&(udpMode==UdpMode::RECEIVE))
        {
            SendOff();
            ReceiveOn();
        }
        m_udpMode = udpMode;
        emit udpModeChanged(m_udpMode);
    }
}

int QudpExtend::numPort() const
{
    //qDebug()<<"numPort = "<<m_numPort;
    return m_numPort;
}

void QudpExtend::setNumPort(const int &nport)
{
    if(m_numPort!=nport)
    {
        m_numPort = nport;
        emit numPortChanged(m_numPort);
        //qDebug()<<"setNumPort = "<<m_numPort;
    }
}

int QudpExtend::ms_send() const
{
    return m_ms_send;
}

void QudpExtend::setMs_send(const int &ms)
{
    if(m_ms_send!=ms)
    {
        m_ms_send = ms;
        emit ms_sendChanged(m_ms_send);
    }
}

QString QudpExtend::hostAddress() const
{
    return m_hostAddress;
}

void QudpExtend::setHostAddress(const QString &host)
{
    if(m_hostAddress!=host)
    {
        m_hostAddress=host;
        emit hostAddressChanged(m_hostAddress);
    }
}

void QudpExtend::setModelToTableView(QTableView *table)
{
    QStringList horizontalHeader;
    horizontalHeader.append(strHeaderBytes);
    horizontalHeader.append(strHeaderType);
    horizontalHeader.append(strHeaderView);
    horizontalHeader.append(strHeaderData);
    model->setHorizontalHeaderLabels(horizontalHeader);

    if(countTable>0) disconnect(tableview,SIGNAL(customContextMenuRequested(QPoint)),0,0);
    if(countTable>0) disconnect(pCombo_DelegateReceiv,SIGNAL(ChangeIndex()),0,0);
    if(countTable>0) disconnect(tCombo_DelegateReceiv,SIGNAL(ChangeIndex()),0,0);

    tableview = table;
    tableview->setModel(model);
    tableview->resizeColumnsToContents();
    // тип
    pCombo_DelegateReceiv = new Combo_Delegate (tableview);
    pCombo_DelegateReceiv->values().insert(s_uint8.toInt(), symb_uint8);
    pCombo_DelegateReceiv->values().insert(s_uint16.toInt(),symb_uint16);
    pCombo_DelegateReceiv->values().insert(s_uint32.toInt(),symb_uint32);
    pCombo_DelegateReceiv->values().insert(s_int8.toInt(),  symb_int8);
    pCombo_DelegateReceiv->values().insert(s_int16.toInt(), symb_int16);
    pCombo_DelegateReceiv->values().insert(s_int32.toInt(), symb_int32);
    tableview->setItemDelegateForColumn(1,pCombo_DelegateReceiv);

    connect(pCombo_DelegateReceiv,SIGNAL(ChangeIndex()),this,SLOT(remapByte()));
    // система счисления
    tCombo_DelegateReceiv = new Combo_Delegate (tableview);
    tCombo_DelegateReceiv->values().insert(s_HEX.toInt(),symb_HEX);
    tCombo_DelegateReceiv->values().insert(s_DEC.toInt(),symb_DEC);
    tCombo_DelegateReceiv->values().insert(s_BIN.toInt(),symb_BIN);
    tableview->setItemDelegateForColumn(2,tCombo_DelegateReceiv); //numSysChangedReceiv()

    connect(tCombo_DelegateReceiv,SIGNAL(ChangeIndex()),this,SLOT(slot_numSysChangedReceiv()));

    //Контекстное меню
    tableview->setContextMenuPolicy(Qt::CustomContextMenu);

    connect(tableview,SIGNAL(customContextMenuRequested(QPoint)),this,SLOT(slotCustomMenuRequested_R(QPoint)));
    countTable++;
}
//-----
void QudpExtend::slot_timer_paint()
{  
    //Чтение таблицы и вывод пакета в таблицу
    if(m_udpMode==UdpMode::RECEIVE)
    {
        emit signal_QuantByte("Кол-во байт: "+QString::number(QuantityBytesIn));
        emit signal_TimePacket("Время: "+QString::number(delay_in)+" мс.");

        int irowCount;
        irowCount = model->rowCount();
        if(irowCount>QuantityBytesIn) irowCount = QuantityBytesIn;

        QuantityBytesAdd = 0;

        quint8 Array[datagram.size()];

        for (int i=0; i<datagram.size();i++)
        {
            Array[i] = datagram.at(i);
        }

        for (int i=0; i<irowCount;i++)
        {
            model->item(i,3)->setText(Convert_receive_Byte(i,Array));
        }
        if(butBlink==0)
        {
            emit signal_ColorBTNChange(QString::fromUtf8("background-color: rgb(124, 205, 124);"));
            butBlink =1;
        }
        else
        {
            emit signal_ColorBTNChange(QString::fromUtf8(""));
            butBlink = 0;
        }
    }
    else if (m_udpMode==UdpMode::SEND)
    {
        emit signal_QuantByte("Байт передано: "+QString::number(QuantityBytesOut));
        if(sendOK)
        {
            if(butBlink==0)
            {
                emit signal_ColorBTNChange(QString::fromUtf8("background-color: rgb(124, 205, 124);"));
                butBlink =1;
            }
            else
            {
                emit signal_ColorBTNChange(QString::fromUtf8(""));
                butBlink = 0;
            }
        }
        else{
            SetMessage("Нет связи. Данные не отправляются");
            emit signal_ColorBTNChange(QString::fromUtf8("background-color: rgb(238,0,0);"));
        }

    }
}
//ВачДог на прием данных// Проверка связи;
void QudpExtend::slot_WatchDog()
{
    timer_paint->stop();
    SetMessage("Нет связи. Данные не приходят. Открыт порт:"+QString::number(m_numPort));
    emit signal_ColorBTNChange(QString::fromUtf8("background-color: rgb(238,0,0);"));
}

void QudpExtend::slot_timer_1sec()
{
    //Вывод количества пакетов в секунду
    //обнуление количества пакетов в сек.
    qDebug()<<"QuantityPacketsIn = "<<QuantityPacketsIn;
    QuantityPacketsIn=0;
}

void QudpExtend::slot_timer_Send()
{
    int irowCount;
    irowCount = tableview->model()->rowCount();
    QString typeText;
    datagram.clear();
    int QuantityBytes_t = 0;
    quint8 Array[QuantityBytesAdd];
    qint64 error;
    bool okey;
    for (int i=0; i<irowCount;i++)
    {
        typeText = model->item(i,1)->text();

        //int8
        if((typeText == s_int8)|(typeText == s_uint8))
        {
            quint8 QUINT8;
           okey = strToArray<quint8>(QUINT8,i,Array,QuantityBytes_t);
            QuantityBytes_t = QuantityBytes_t + 1;
        }
        // int16
        else if((typeText == s_int16)|(typeText == s_uint16))
        {
            quint16 QUINT16;
           okey = strToArray<quint16>(QUINT16,i,Array,QuantityBytes_t);
            QuantityBytes_t+=2;
        }
        //int32
        else if((typeText == s_int32)|(typeText == s_uint32))
        {
            quint32 QUINT32;
           okey = strToArray<quint32>(QUINT32,i,Array,QuantityBytes_t);
            QuantityBytes_t+=4;
        }
        if(!okey)model->item(i,3)->setBackground(Qt::red);
        else model->item(i,3)->setBackground(Qt::white);
    }

    for(int i=0;i<QuantityBytesAdd;i++)
    {
        datagram.append(Array[i]);
    }

    QHostAddress HOST;
    HOST.setAddress(m_hostAddress);

    error = udpSocket->writeDatagram(datagram.data(), datagram.size(),HOST, m_numPort);

    QuantityBytesOut = error ;// SetMessage("Number bytes sent on success " +QString::number(error) );

    if(error>0){
        sendOK = true;
    }
    else sendOK = false;
}

void QudpExtend::slot_numSysChangedReceiv()
{
    int irow = tableview->currentIndex().row();
    fncNumSysChange(tCombo_DelegateReceiv,irow);
}

void QudpExtend::slotCustomMenuRequested_R(QPoint pos)
{
    QMenu *menu = new QMenu(tableview);
    QAction *act_TypeAll = new QAction(("Применить тип для всех"),tableview);
    QAction *act_ViewAll = new QAction(("Применить вид для всех"),tableview);

    connect(act_TypeAll,SIGNAL(triggered(bool)),this,SLOT(slotTypeAll()));
    connect(act_ViewAll,SIGNAL(triggered(bool)),this,SLOT(slotViewAll()));

    menu->addAction(act_TypeAll);
    menu->addAction(act_ViewAll);

    menu->popup(tableview->viewport()->mapToGlobal(pos));
}

void QudpExtend::slotTypeAll()
{
    if(model->rowCount()<1)return;
    int row = tableview->selectionModel()->currentIndex().row();
    QString TypeText = model->item(row,1)->text();
    int countRow = model->rowCount();
    QuantityBytesAdd = 0;
    for(int i=0;i<countRow;i++)
    {
        model->item(i,1)->setText(TypeText);
        remap(TypeText,&QuantityBytesAdd,i);
    }
}

void QudpExtend::slotViewAll()
{
    if(model->rowCount()<1)return;
    int row = tableview->selectionModel()->currentIndex().row();
    QString ViewText = model->item(row,2)->text();
    int countRow = model->rowCount();
    for(int i=0;i<countRow;i++)
    {
        model->item(i,2)->setText(ViewText);
    }
}

void QudpExtend::slotAddRow()
{
    AddRow(&QuantityBytesAdd,QString::number(QuantityBytesAdd),"1","10",QString::number(QuantityBytesAdd));
}
// Включение Запись Пакетов
void QudpExtend::slotRecordOn()
{
    if(Record_On == 0)
    {
        QString pathSave;
        if(csvRec())//(ui->radioButton_csv->isChecked())
        {
             pathSave=QGuiApplication::applicationDirPath()+QDir::separator()+ "LOG_Receive.csv";
        }
        else pathSave=QGuiApplication::applicationDirPath()+QDir::separator()+ "LOG_Receive.txt";

        File_Record = new QFile(pathSave);
        if(!File_Record->open(QFile::WriteOnly | QFile::Text))
        {
            SetMessage("Файл для записи не открыть");
        }
        else
        {
            outstream = new QTextStream(File_Record);
            outstream->setCodec("UTF-8");
            Record_On = 1;
            SetMessage("Запись включена");
            emit signal_EnableChanged(!Record_On);    
        }

    }
    else
    {
        File_Record->close();
        delete File_Record;
        delete outstream;
        Record_On = 0;
        SetMessage("Запись выключена");
        emit signal_EnableChanged(!Record_On);    
    }
}
// Открыть файл записи Лога
void QudpExtend::slotOpenRecord()
{
    QString path;
    if(csvRec())path =QGuiApplication::applicationDirPath()+QDir::separator()+ "LOG_Receive.csv";
    if(!csvRec())path =QGuiApplication::applicationDirPath()+QDir::separator()+ "LOG_Receive.txt";
    QDesktopServices::openUrl(QUrl("file:///"+path));
}

void QudpExtend::slotSaveXML(const QString &filename)
{
    QString path;
    if(filename =="")path = QDir::currentPath()+QDir::separator()+"settings.xml";
    else path = filename;
    QFile file(path);

    file.open(QIODevice::WriteOnly);

    QXmlStreamWriter xmlWriter(&file); // объект для записи в файл
    xmlWriter.setAutoFormatting(true);// Автоформатирование текста
    xmlWriter.writeStartDocument();// Запуск записи в документ
    xmlWriter.writeStartElement("settings"); //запись элемента настроек

        /*Настройки UDP*/
        xmlWriter.writeStartElement("udp");
        xmlWriter.writeAttribute("numPort",QString::number(numPort()));
        xmlWriter.writeAttribute("hostAddress",hostAddress());
        xmlWriter.writeAttribute("csvRec",QString::number(csvRec()));
        xmlWriter.writeAttribute("convertRec",QString::number(convertRec()));
        xmlWriter.writeAttribute("ms_send",QString::number(ms_send()));
        xmlWriter.writeEndElement();   

    //Записываем на строки пакетов приема
    if(model->rowCount()!=0)
    {
        for(int i=0;i<model->rowCount();i++)
        {
            if(i!=0)xmlWriter.writeStartElement("row"+QString::number(i));
            else xmlWriter.writeStartElement("row");
            xmlWriter.writeAttribute("bytes",model->item(i,0)->text());
            xmlWriter.writeAttribute("type",model->item(i,1)->text());
            xmlWriter.writeAttribute("view",model->item(i,2)->text());
            xmlWriter.writeAttribute("data",model->item(i,3)->text());
            xmlWriter.writeEndElement();
        }
    }
    /*Закрываем элемент настроек  settings */
    xmlWriter.writeEndElement();
    xmlWriter.writeEndDocument();
    file.close(); // Закрываем файл
}

void QudpExtend::slotLoadXML(const QString &filename)
{
    QString path;
    if(filename =="")path = QDir::currentPath()+QDir::separator()+"settings.xml";
    else path = filename;

    QFile file(path);
    if(!file.open(QFile::ReadOnly|QFile::Text))
    {
        /*QMessageBox::warning(this,
                             "Ошибка файла",
                             "Не удалось открыть файл "+path,
                             QMessageBox::Ok);*/
        SetMessage("Не удалось открыть файл "+path);
    }
    else
    {
        /*Создаем объект для чтения файла*/
        QXmlStreamReader xmlReader;
        xmlReader.setDevice(&file);
        xmlReader.readNext(); // Переход к первому элементу в файле
        int i0 = 0;// счетчик строк в таблице приема

        /*Цикл до тех пор пока не дойдем до конца файла*/
        while(!xmlReader.atEnd())
        {
            if(xmlReader.name() == "udp")
            {
                foreach (const QXmlStreamAttribute &attr,xmlReader.attributes())
                {
                    if(attr.name().toString() == "numPort")
                    {
                        int attribute_value = attr.value().toInt();
                        setNumPort(attribute_value);
                    }
                    if(attr.name().toString() == "hostAddress")
                    {
                        QString attribute_value = attr.value().toString();
                        setHostAddress(attribute_value);
                    }
                    if(attr.name().toString() == "csvRec")
                    {
                        int attribute_value = attr.value().toInt();
                        setCsvRec(attribute_value);
                    }
                    if(attr.name().toString() == "convertRec")
                    {
                        int attribute_value = attr.value().toInt();
                        setConvertRec(attribute_value);
                    }
                    if(attr.name().toString() == "ms_send")
                    {
                        int attribute_value = attr.value().toInt();
                        setMs_send(attribute_value);
                    }
                }
            }            
            // настройки таблицы
            if(((i0!=0)&(xmlReader.name()=="row"+QString::number(i0)))|((i0==0)&(xmlReader.name()=="row")))
            {
                QString attr_bytes,attr_type,attr_view,attr_data;
                foreach (const QXmlStreamAttribute &attr,xmlReader.attributes())
                {
                    if(attr.name().toString() == "bytes")
                    {
                        attr_bytes = attr.value().toString();
                    }
                    if(attr.name().toString() == "type")
                    {
                        attr_type = attr.value().toString();
                    }
                    if(attr.name().toString() == "view")
                    {
                        attr_view = attr.value().toString();
                    }
                    if(attr.name().toString() == "data")
                    {
                        attr_data = attr.value().toString();
                    }
                }
                AddRow(&QuantityBytesAdd,attr_bytes,attr_type,attr_view,attr_data);
                i0++;
            }
            xmlReader.readNext();
        }
    }
    file.close();
}
void QudpExtend::fncNumSysChange(Combo_Delegate *comboDelegate,int n_row)
{
    /*1,"uint8"
      2,"uint16"
      3,"uint32"
      4,"int8"
      5,"int16"
      6,"int32"*/

    QString newTextCombo = comboDelegate->curText;
    if(s_uint8==model->item(n_row,1)->text())
    {
        quint8 QUINT8_T;
        transNumSys<quint8>(QUINT8_T,newTextCombo,n_row);
    }
    else if(s_uint16==model->item(n_row,1)->text())
    {
        quint16 QUINT16_T;
        transNumSys<quint16>(QUINT16_T,newTextCombo,n_row);
    }
    else if(s_uint32==model->item(n_row,1)->text())
    {
        quint32 QUINT32_T;
        transNumSys<quint32>(QUINT32_T,newTextCombo,n_row);
    }
    else if(s_int8==model->item(n_row,1)->text())
    {
        qint8 QINT8_T;
        transNumSys<qint8>(QINT8_T,newTextCombo,n_row);

    }
    else if(s_int16==model->item(n_row,1)->text())
    {
        qint16 QINT16_T;
        transNumSys<qint16>(QINT16_T,newTextCombo,n_row);
    }
    else if(s_int32==model->item(n_row,1)->text())
    {
        qint32 QINT32_T;
        transNumSys<qint32>(QINT32_T,newTextCombo,n_row);
    }
}

//---------- смена системы счисления--- включается при смене по выпадающему списку таблицы
template <typename T> void QudpExtend::transNumSys(T &num, QString newText, int row)
{
    bool ok;
    QString numComboIndex;
    QString prevText = model->item(row,2)->text();
    int NumSize = sizeof(num);//размер типа
    int QuantSymbol;
    if(model->item(row,3)->text()=="")return;
    if(prevText != s_HEX)
    {
        num = model->item(row,3)->text().toInt(&ok,prevText.toInt());
        if(ok==0) num = model->item(row,3)->text().toUInt(&ok,prevText.toInt());
    }

    else
    {
        QString HexSTRING = model->item(row,3)->text();
        int temp = HexToInt(HexSTRING,NumSize);
        memcpy(&num,&temp,sizeof(num));
    }
    if(newText == symb_BIN)
    {
        numComboIndex = s_BIN;
        QuantSymbol = NumSize * 8;
    }
    else if(newText == symb_HEX)
    {
        numComboIndex = s_HEX;
        QuantSymbol = NumSize * 2;
    }
    else if(newText == symb_DEC) numComboIndex = s_DEC;

    if(prevText!=numComboIndex)
    {
        QString TextCombo, strNumber;
        strNumber = QString::number(num,numComboIndex.toInt()).toUpper();
        //******
        TextCombo = Convert_receive_Byte_Extend(strNumber,numComboIndex,NumSize);
        model->item(row,2)->setText(numComboIndex);
        if(numComboIndex != s_DEC) model->item(row,3)->setText(TextCombo.right(QuantSymbol));
        else model->item(row,3)->setText(TextCombo);
    }
}
//Получили пакет
void QudpExtend::processPendingDatagrams()
{
    datagram.clear();
    QDateTime curTime;
    curTime = QDateTime::currentDateTime();
    if(!simOn){
        while (udpSocket->hasPendingDatagrams())
        {
            datagram.resize(udpSocket->pendingDatagramSize());
            udpSocket->readDatagram(datagram.data(),datagram.size());
        }
    }

    if(simOn){
        for (int i=0; i<simulateUDP->dataArray.size();i++)
        {
            datagram.append(simulateUDP->dataArray.at(i));
        }
    }
    QuantityBytesIn = datagram.size();
    delay_in = timer->elapsed();// получить время между пакетами
    timer->restart();
    WathDog_Receive->start(wachdog_delay);// перезапустить ватчдог на прием данных
    if(!timer_paint->isActive())timer_paint->start(ms_paint); // перезапуск таймера мигания кнопки
    // Copy in Array
    quint8 Array[datagram.size()];
    for (int i=0; i<datagram.size();i++)
    {
        Array[i] = datagram.at(i);
    }
    //---включение записи лога
    if(Record_On)
    {
        QString datagram_txt = 0;
        quint8 udata8;
        QTime Time;
        Time = QTime::currentTime();
        datagram_txt = Time.toString("hh:mm:ss:zzz");
        QString separ;
        if(m_csvRec)// if csv true
        {
            separ = "\t";
        }
        else separ = " ";
        if(!m_convertRec)// if NATIVE
        {
            for (int i=0; i<datagram.size();i++)
            {
            udata8 = datagram.at(i);
            datagram_txt+= separ+QString::number(udata8,16).toUpper();
            }
        }

       else if(m_convertRec) // Включено конвертирование по таблице
       {
           //Чтение таблицы и вывод пакета в таблицу
           int irowCount;
           irowCount = model->rowCount();
           if(irowCount>QuantityBytesIn) irowCount = QuantityBytesIn;

           QuantityBytesAdd = 0;

           for (int i=0; i<irowCount;i++)
           {
              datagram_txt+= separ+Convert_receive_Byte(i,Array);
           }
       }
        outstream->operator <<( datagram_txt);
        outstream->operator <<("\r\n");
        str_datagram = datagram_txt;
    }
    QuantityPacketsIn++;// увеличение индекса пакетов в сек.
}

void QudpExtend::AddRow(int *QuantityBytes, QString sBytes, QString sType, QString sView, QString Data)
{
    int irow = model->rowCount();

    item = new QStandardItem(sBytes);
    model->setItem(irow, 0, item);
    item = new QStandardItem(sType);
    model->setItem(irow, 1, item);
    item = new QStandardItem(sView);
    model->setItem(irow, 2, item);
    item = new QStandardItem(Data);
    model->setItem(irow, 3, item);

    remap(sType,QuantityBytes,irow);
    //*QuantityBytes = *QuantityBytes + 1;
}

void QudpExtend::deleteRowModel()
{
    int irow = model->rowCount();

    QString typeText;
    int typesize=0;

    if(irow<1)return;
    typeText = model->item(irow-1,1)->text();
    if((typeText == s_uint8)|(typeText == s_int8))
    {
        typesize=1;
    }
    else if((typeText == s_uint16)|(typeText == s_int16))
    {
        typesize=2;
    }
    else if((typeText == s_uint32)|(typeText == s_int32))
    {
        typesize=4;
    }
    QuantityBytesAdd= QuantityBytesAdd - typesize;
    tableview->model()->removeRow(irow-1,QModelIndex());
}

QString QudpExtend::Convert_receive_Byte(int i, quint8 *Array)
{
    QString typeText, typeDisplay;
    QString str_return = 0;
    int oldQuantity = QuantityBytesAdd ;
    typeText = model->item(i,1)->text();
    typeDisplay = model->item(i,2)->text();
    if(QuantityBytesAdd<datagram.size()){

    if(typeText == s_uint8)
    {
        quint8 udata8;
        udata8 = datagram.at(QuantityBytesAdd);
        str_return = QString::number(udata8,typeDisplay.toInt()).toUpper();
        QuantityBytesAdd= QuantityBytesAdd + 1;
    }
    else if(typeText == s_int8)
    {
        qint8 data8;
        data8 = datagram.at(QuantityBytesAdd);
        str_return =QString::number(data8,typeDisplay.toInt()).toUpper();
        QuantityBytesAdd= QuantityBytesAdd + 1;

    }
    else if(typeText == s_uint16)
    {
        quint16 udata16;
        memcpy(&udata16,&Array[QuantityBytesAdd],sizeof(quint16));
        str_return =QString::number(udata16,typeDisplay.toInt()).toUpper();

        QuantityBytesAdd+=2;
    }
    else if(typeText == s_int16)
    {
        qint16 data16;
        memcpy(&data16,&Array[QuantityBytesAdd],sizeof(qint16));
        str_return =QString::number(data16,typeDisplay.toInt()).toUpper();

        QuantityBytesAdd+=2;
    }
    else if(typeText == s_uint32)
    {
       quint32 udata32;
       memcpy(&udata32,&Array[QuantityBytesAdd],sizeof(quint32));
       str_return =QString::number(udata32,typeDisplay.toInt()).toUpper();
       QuantityBytesAdd+=4;
    }
    else if(typeText == s_int32)
    {
        qint32 data32;
        memcpy(&data32,&Array[QuantityBytesAdd],sizeof(qint32));
        str_return =QString::number(data32,typeDisplay.toInt()).toUpper();
        QuantityBytesAdd+=4;
    }
    }
    QString convStr = Convert_receive_Byte_Extend(str_return,typeDisplay,(QuantityBytesAdd-oldQuantity));
    return(convStr);
}

QString QudpExtend::Convert_receive_Byte_Extend(QString str_return, QString typeDisplay, int isize)
{
    QString str=str_return;
    int bSize = isize*8;
    int hSize = isize*2;

    if(typeDisplay.toInt()==16)
    {
        if(str_return.size()>hSize)         str= str_return.right(hSize+1);
        else if(str_return.size()<hSize)    str = str_return.prepend(QString("0"));
    }
    else if(typeDisplay.toInt()==2)
    {
        if(str_return.size()>bSize)str = str_return.right(bSize+1);
        else if(str_return.size()<bSize){
            int leght = bSize-str_return.size();
            for(int i=0;i<leght;i++)
            {
                str = str_return.prepend(QString("0"));
            }
        }
    }
    return str;
}

void QudpExtend::remap(QString ComboText, int *quantity, int row)
{
    if((ComboText == symb_int8)|(ComboText == symb_uint8)|(ComboText == s_int8)|(ComboText == s_uint8))
    {
        model->item(row,0)->setText(QString::number(*quantity));
        *quantity = *quantity + 1;
    }
    else if((ComboText == symb_int16)|(ComboText == symb_uint16)|(ComboText == s_int16)|(ComboText == s_uint16))
    {
        model->item(row,0)->setText(QString::number(*quantity)+", "+QString::number(*quantity+1));
        *quantity+=2;
    }
    else if((ComboText == symb_int32)|(ComboText == symb_uint32)|(ComboText == s_int32)|(ComboText == s_uint32))
    {
        model->item(row,0)->setText(QString::number(*quantity)+", "+
                                  QString::number(*quantity+1)+", "+
                                  QString::number(*quantity+2)+", "+QString::number(*quantity+3));
        *quantity+=4;
     }
    QString newComboText ="";
    if(ComboText == symb_uint8)        newComboText = s_uint8;
    else if(ComboText == symb_uint16)  newComboText = s_uint16;
    else if(ComboText == symb_uint32)  newComboText = s_uint32;
    else if(ComboText == symb_int8)    newComboText = s_int8;
    else if(ComboText == symb_int16)   newComboText = s_int16;
    else if(ComboText == symb_int32)   newComboText = s_int32;
    if(newComboText!="") model->item(row,1)->setText(newComboText);
}

void QudpExtend::remapByte()
{
    QString typeText;
    QuantityBytesAdd = 0;

    int irow = tableview->currentIndex().row();
    QString QBytesAdd = model->item(irow,0)->text();
    QStringList listBytes = QBytesAdd.split(", ");

    QuantityBytesAdd = listBytes.at(0).toInt();
    typeText = pCombo_DelegateReceiv->curText;

    remap(typeText,&QuantityBytesAdd,irow);
    model->item(irow,1)->setText(typeText);

    int countRow = model->rowCount();

    for(int i=irow+1;i<countRow;i++)
    {
        typeText = model->item(i,1)->text();
        remap(typeText,&QuantityBytesAdd,i);
    }
}

void QudpExtend::ReceiveOn()
{
    if(!simOn)udpSocket->bind(m_numPort,QUdpSocket::ShareAddress);//
    else simulateUDP->bind();
    QuantityBytesIn = 0;
    delay_in = 0;
    datagram.clear();
    timer_paint->start(ms_paint);
    timer->start();
    WathDog_Receive->start(wachdog_delay);
    timer_1sec->start(1000);
    SetMessage("Прием включен. Открыт порт:"+QString::number(m_numPort));
}

void QudpExtend::ReceiveOff()
{
    if(!simOn) udpSocket->abort();
    else simulateUDP->abort();
    timer_paint->stop();
    WathDog_Receive->stop();
    timer_1sec->stop();
    SetMessage("Прием выключен");
    butBlink = 0;
    emit signal_ColorBTNChange(QString::fromUtf8(""));
    emit signal_QuantByte("Кол-во байт: ");
    emit signal_TimePacket("Время: ");
}

void QudpExtend::SendOn()
{
    timer_Send->start(m_ms_send);
    timer_paint->start(ms_paint);
    SetMessage("Передача включена. Открыт порт:"+QString::number(m_numPort)+" IP:"+m_hostAddress);
    //qDebug()<<m_message;
   //qDebug()<<"m_ms_send = "<<m_ms_send;
}

void QudpExtend::SendOff()
{
    udpSocket->abort();

    timer_Send->stop();

    timer_paint->stop();

    SetMessage("Передача выключена");
    butBlink = 0;
    emit signal_ColorBTNChange(QString::fromUtf8(""));
}

void QudpExtend::SetMessage(const QString &message)
{
    m_message = message;
    emit signal_MessageChanged(m_message);
}

//Перевод строки QString в Массив QUINT8// Преобразование данных дя передачи
template <typename T> bool QudpExtend::strToArray(T &num, int i, quint8 *Array, int N_Bytes)
{
    QString str;
    bool ok;
    QString SysType = model->item(i,2)->text();
    int numSys = SysType.toInt();
    str = model->item(i,3)->text();
    num = str.toInt(&ok,numSys);
    if(!ok)num = 0;
    memcpy(&Array[N_Bytes],&num,sizeof(num));
    return(ok);
}
