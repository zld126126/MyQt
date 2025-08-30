#include "mycamera.h"
#include "ui_mycamera.h"
#include <QDebug>
#include <QDateTime>
#include <QString>
#include <QDir>
#include <QPixmap>

static int capNum = 0;

MyCamera::MyCamera(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MyCamera)
{
    ui->setupUi(this);

    //获取系统可用摄像头信息
    QList<QCameraInfo> infos = QCameraInfo::availableCameras();
    //显示到下拉列表
    foreach(QCameraInfo info,infos){
        qDebug()<<info.deviceName()<<info.description();
        //把摄像头设备名加入列表
        ui->comboBox->addItem(info.deviceName());
    }

    ui->pushButton_stop->setEnabled(false);
}

MyCamera::~MyCamera()
{
    delete ui;
}

void MyCamera::on_pushButton_stop_clicked()
{
    //停止摄像头
    camera->stop();
    //隐藏显示窗口
    cameraViewfinder->hide();

    delete camera;
    delete cameraViewfinder;
    delete cameraImageCapture;

    ui->pushButton_start->setEnabled(true);
    ui->pushButton_stop->setEnabled(false);
}

void MyCamera::on_pushButton_start_clicked()
{
    //构造摄像头对象
    camera = new QCamera(ui->comboBox->currentText().toUtf8(),this);
    //构造显示窗口对象
    cameraViewfinder = new QCameraViewfinder(ui->widget);
    //调整大小
    cameraViewfinder->resize(ui->widget->size());
    //构造截屏对象
    cameraImageCapture = new QCameraImageCapture(camera,this);
    //注册截图显示
    connect(cameraImageCapture, &QCameraImageCapture::imageCaptured, this, &MyCamera::set_pix);

    //设置摄像头的显示窗口
    camera->setViewfinder(cameraViewfinder);
    cameraViewfinder->show();

    //启动摄像头
    camera->start();

    //禁止开始，使能停止
    ui->pushButton_start->setEnabled(false);
    ui->pushButton_stop->setEnabled(true);
}

QString MyCamera::generateCaptureFileName(){
    // 获取当前时间，精确到秒
   QDateTime currentTime = QDateTime::currentDateTime();

   // 格式化时间为 "yyyyMMddhhmmss" 格式
   QString timeStr = currentTime.toString("yyyyMMddhhmmss");

   // 计数器自增
   capNum++;
   //qDebug() << "截图次数:" << capNum << endl;

   // 组合成完整文件名
   return QString("%1_%2.%3").arg(timeStr).arg(capNum).arg("jpg");
}

void MyCamera::set_pix(int id, const QImage &preview){
    ui->label_capture->setPixmap(QPixmap::fromImage(preview).scaled(ui->label_capture->size()));
}

void MyCamera::on_pushButton_cap_clicked()
{
    qDebug()<<QDir::currentPath();
    currentCaptureFileName = QDir::currentPath() + "/" + generateCaptureFileName();
    cameraImageCapture->capture(currentCaptureFileName);
    qDebug() << "截图:" << currentCaptureFileName << endl;
}

