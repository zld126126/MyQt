#ifndef MYSERIAL_H
#define MYSERIAL_H

#include <QWidget>
#include <QSerialPortInfo>
#include <QSerialPort>

QT_BEGIN_NAMESPACE
namespace Ui { class MySerial; }
QT_END_NAMESPACE

class MySerial : public QWidget
{
    Q_OBJECT

public:
    MySerial(QWidget *parent = nullptr);
    ~MySerial();

private slots:
    void on_pushButton_openclose_clicked();

    void on_pushButton_send_clicked();
    //接收串口数据槽函数
    void readData();

private:
    Ui::MySerial *ui;
    //串口对象
    QSerialPort *ser;
};
#endif // MYSERIAL_H
