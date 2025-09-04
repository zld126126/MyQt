#include "myserial.h"
#include "ui_myserial.h"
#include <QDebug>
#include <QMessageBox>

MySerial::MySerial(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::MySerial)
{
    ui->setupUi(this);

    //获取系统所有可用串口
    foreach(const QSerialPortInfo &info,QSerialPortInfo::availablePorts()){
        qDebug()<<info.portName()<<info.description();
        //测试串口是否可用
        QSerialPort serial(info);
        if(serial.open(QIODevice::ReadWrite)){//空闲
            //将串口名加入combox
            ui->comboBox_port->addItem(serial.portName());
            //关闭串口
            serial.close();
        }
        else{//被占用
            ui->comboBox_port->addItem(serial.portName()+"(被占用)");
        }
    }

    ui->pushButton_send->setEnabled(false);
}


MySerial::~MySerial()
{
    delete ui;
}

//打开/关闭串口
void MySerial::on_pushButton_openclose_clicked()
{
    if(ui->pushButton_openclose->text()=="打 开 串 口"){//打开
        ser = new QSerialPort(ui->comboBox_port->currentText(),this);
        //打开
        if(!ser->open(QIODevice::ReadWrite)){
            QMessageBox::warning(this,"提示","打开失败",QMessageBox::Ok);
            ser->deleteLater();
            return;
        }

        //设置波特率
        switch(ui->comboBox_baudrate->currentIndex()){
            case 0:
                ser->setBaudRate(QSerialPort::Baud115200);
                break;
            case 1:
                ser->setBaudRate(QSerialPort::Baud2400);
                break;
            case 2:
                ser->setBaudRate(QSerialPort::Baud4800);
                break;
            case 3:
                ser->setBaudRate(QSerialPort::Baud9600);
                break;
            case 4:
                ser->setBaudRate(QSerialPort::Baud38400);
                break;
        }

        //设置数据位位数
        switch(ui->comboBox_databits->currentIndex()){
            case 0:
                ser->setDataBits(QSerialPort::Data8);
                break;
            case 1:
                ser->setDataBits(QSerialPort::Data5);
                break;
            case 2:
                ser->setDataBits(QSerialPort::Data6);
                break;
            case 3:
                ser->setDataBits(QSerialPort::Data7);
                break;
        }

        //设置校验方式
        switch(ui->comboBox_parity->currentIndex()){
            case 0:
                ser->setParity(QSerialPort::NoParity);
                break;
            case 1:
                ser->setParity(QSerialPort::EvenParity);
                break;
            case 2:
                ser->setParity(QSerialPort::OddParity);
                break;
        }

        //设置停止位位数
        switch(ui->comboBox_stopbits->currentIndex()){
            case 0:
                ser->setStopBits(QSerialPort::OneStop);
                break;
            case 1:
                ser->setStopBits(QSerialPort::OneAndHalfStop);
                break;
            case 2:
                ser->setStopBits(QSerialPort::TwoStop);
                break;
        }

        //关闭流控
        ser->setFlowControl(QSerialPort::NoFlowControl);

        //关闭combox
        ui->comboBox_port->setEnabled(false);
        ui->comboBox_baudrate->setEnabled(false);
        ui->comboBox_databits->setEnabled(false);
        ui->comboBox_parity->setEnabled(false);
        ui->comboBox_stopbits->setEnabled(false);

        ui->pushButton_send->setEnabled(true);

        //修改按钮显示
        ui->pushButton_openclose->setText("关 闭 串 口");

        //连接信号和槽
        QObject::connect(ser,&QSerialPort::readyRead,this,&MySerial::readData);
    }
    else{//关闭
        ser->clear();
        ser->close();
        ser->deleteLater();

        ui->comboBox_port->setEnabled(true);
        ui->comboBox_baudrate->setEnabled(true);
        ui->comboBox_databits->setEnabled(true);
        ui->comboBox_parity->setEnabled(true);
        ui->comboBox_stopbits->setEnabled(true);

        ui->pushButton_send->setEnabled(false);

        //修改按钮显示
        ui->pushButton_openclose->setText("打 开 串 口");
    }
}

//发送数据
void MySerial::on_pushButton_send_clicked()
{
    //获取发送的内容
    QByteArray data = ui->textEdit->toPlainText().toUtf8();
    ser->write(data);
}

//接收数据
void MySerial::readData()
{
    //接收所有数据
    QByteArray data = ser->readAll();

    if(!data.isEmpty()){
        ui->textBrowser->append(data);
    }
}
