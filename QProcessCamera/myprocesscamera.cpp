#include "myprocesscamera.h"
#include "ui_myprocesscamera.h"
#include <QDebug>

MyProcessCamera::MyProcessCamera(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::MyProcessCamera)
{
    ui->setupUi(this);

    pro = new QProcess(this);

    ui->pushButton_start->setEnabled(true);
    ui->pushButton_stop->setEnabled(false);
    ui->pushButton_pause->setEnabled(false);
}

MyProcessCamera::~MyProcessCamera()
{
    delete ui;
}

//开始
void MyProcessCamera::on_pushButton_start_clicked()
{
    if(pro->state() == QProcess::NotRunning) {
        QString cmd = "/software/camera_capture";
        QStringList args;
        // 注意：后台运行符号&不需要作为参数传递，QProcess本身可以管理进程生命周期
        // args << "&";  // 这行可以去掉

        pro->start(cmd, args);

        // 检查是否启动成功
        if (!pro->waitForStarted()) {  // 等待进程启动，超时默认30秒
            qDebug() << "启动失败：" << pro->errorString();
        } else {
            qDebug() << "进程已启动，PID：" << pro->pid();
        }
    }

    ui->pushButton_start->setEnabled(false);
    ui->pushButton_stop->setEnabled(true);
    ui->pushButton_pause->setEnabled(true);
}

//停止
void MyProcessCamera::on_pushButton_stop_clicked()
{
    if(pro->state()==QProcess::Running){
        pro->kill();
        pro->waitForFinished();
    }

    ui->pushButton_start->setEnabled(true);
    ui->pushButton_stop->setEnabled(false);
    ui->pushButton_pause->setEnabled(false);
}

//暂停
void MyProcessCamera::on_pushButton_pause_clicked()
{
    QString cmd = QString("killall");
    QStringList args;

    if(ui->pushButton_pause->text()=="暂停"){
        args<<"-STOP"<<"camera_test"<<"&";
        ui->pushButton_pause->setText("继续");
    }
    else if(ui->pushButton_pause->text()=="继续"){
        args<<"-CONT"<<"camera_test"<<"&";
        ui->pushButton_pause->setText("暂停");
    }

    //停止执行当前程序，执行指定的内容，执行完成后继续执行原程序
    pro->execute(cmd,args);
}

