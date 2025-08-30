#include "mythreadcamera.h"
#include "ui_mythreadcamera.h"
#include <QImage>
#include <QPixmap>
#include <QDebug>
#include <QFileDialog>
#include <QDateTime>
#include <QMessageBox>
#include <QThread>
#include <QMutexLocker>

// 声明C接口
extern "C" {
    int camera_init();          // 初始化摄像头
    int camera_capture();       // 捕获一帧（生成RGB数据）
    unsigned char* get_rgb_data(); // 获取RGB数据指针
    unsigned int get_rgb_data_len(); // 获取RGB数据长度
    void free_rgb_data();       // 释放RGB数据
    void camera_cleanup();      // 清理摄像头资源
}

// CameraThread类实现
CameraThread::CameraThread(QObject *parent)
    : QThread(parent)
    , m_isRunning(false)
    , m_rgbData(nullptr)
    , m_rgbDataLen(0)
{
}

CameraThread::~CameraThread()
{
    stop();
    wait(); // 等待线程结束
}

void CameraThread::stop()
{
    m_isRunning = false;
}

unsigned char* CameraThread::getRGBData() const
{
    QMutexLocker locker(&m_mutex);
    return m_rgbData;
}

unsigned int CameraThread::getRGBDataLen() const
{
    QMutexLocker locker(&m_mutex);
    return m_rgbDataLen;
}

void CameraThread::run()
{
    // 在线程中初始化摄像头
    m_isRunning = true;
    int initResult = camera_init();
    bool success = (initResult == 0);
    
    // 发送初始化完成信号
    emit initFinished(success);
    
    if (!success) {
        return;
    }
    
    while (m_isRunning) {
        // 调用C接口捕获一帧（生成RGB数据）
        int captureRet = camera_capture();
        if (captureRet == 0) {
            // 获取RGB数据（调用C接口）并更新线程内的成员变量
            QMutexLocker locker(&m_mutex);
            m_rgbData = get_rgb_data();
            m_rgbDataLen = get_rgb_data_len();
            
            // 释放锁后再发送信号，避免死锁
            locker.unlock();
            
            // 发送帧捕获完成信号
            emit frameCaptured();
        } else {
            qWarning() << "Capture frame failed in thread, code:" << captureRet;
        }
        
        // 控制帧率（约10fps）
        msleep(100);
    }
    
    // 清理摄像头资源
    camera_cleanup();
    free_rgb_data();
}

// MyThreadCamera类实现
MyThreadCamera::MyThreadCamera(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MyThreadCamera)
    , m_isCapturing(false)
    , m_initSuccess(false)
{
    ui->setupUi(this);

    m_cameraLabel = ui->label;
    m_cameraLabel->setAlignment(Qt::AlignCenter);
    m_cameraLabel->setText("点击开始按钮启动摄像头...");

    // 初始化摄像头线程
    m_cameraThread = new CameraThread(this);
    connect(m_cameraThread, &CameraThread::frameCaptured, this, &MyThreadCamera::onFrameCaptured);
    connect(m_cameraThread, &CameraThread::initFinished, this, &MyThreadCamera::onInitFinished);
}

MyThreadCamera::~MyThreadCamera()
{
    stopCapture();
    
    // 清理资源
    delete m_cameraThread;
    delete ui;
}

// 处理帧捕获完成的槽函数
void MyThreadCamera::onFrameCaptured()
{
    // 获取RGB数据
    unsigned char *rgbData = m_cameraThread->getRGBData();
    unsigned int rgbLen = m_cameraThread->getRGBDataLen();
    
    if (rgbData == NULL || rgbLen == 0) {
        qWarning() << "RGB data is empty!";
        return;
    }

    // 保存当前帧用于截图
    m_currentImage = QImage(rgbData,
                           640,
                           480,
                           640 * 3,
                           QImage::Format_RGB888);

    // 显示在标签上
    QPixmap pixmap = QPixmap::fromImage(m_currentImage);
    pixmap = pixmap.scaled(m_cameraLabel->size(),
                          Qt::KeepAspectRatio,
                          Qt::SmoothTransformation);

    m_cameraLabel->setPixmap(pixmap);
}

// 处理初始化完成的槽函数
void MyThreadCamera::onInitFinished(bool success)
{
    m_initSuccess = success;
    if (!success) {
        m_cameraLabel->setText("摄像头初始化失败！");
        ui->pushButton_start->setEnabled(false);
        qCritical() << "Camera init failed";
    }
}

// 开始捕获按钮点击事件
void MyThreadCamera::on_pushButton_start_clicked()
{
    if (!m_isCapturing && !m_cameraThread->isRunning()) {
        m_cameraThread->start();
        m_isCapturing = true;
        ui->pushButton_start->setText("正在捕获...");
        ui->pushButton_start->setEnabled(false);
        ui->pushButton_stop->setEnabled(true);
        m_cameraLabel->setText("正在初始化摄像头并显示画面...");
    }
}

// 停止捕获按钮点击事件
void MyThreadCamera::on_pushButton_stop_clicked()
{
    if (m_isCapturing) {
        stopCapture();

        // 截图并保存
        if (!m_currentImage.isNull()) {
            saveScreenshot();
        }
    }
}

// 停止捕获
void MyThreadCamera::stopCapture()
{
    if (m_isCapturing) {
        m_cameraThread->stop();
        m_cameraThread->wait(); // 等待线程结束
        m_isCapturing = false;
        ui->pushButton_start->setText("开始捕获");
        ui->pushButton_start->setEnabled(true);
        ui->pushButton_stop->setEnabled(false);
        m_cameraLabel->setText("已停止捕获，截图已保存");
    }
}

// 保存截图
void MyThreadCamera::saveScreenshot()
{
    // 生成带时间戳的文件名
    QString timestamp = QDateTime::currentDateTime().toString("yyyyMMdd_HHmmss");
    QString defaultFileName = QString("screenshot_%1.jpg").arg(timestamp);

    // 打开文件对话框
    QString fileName = QFileDialog::getSaveFileName(
        this,
        "保存截图",
        defaultFileName,
        "JPEG图片 (*.jpg *.jpeg);;所有文件 (*)"
    );

    if (!fileName.isEmpty()) {
        // 保存图片
        if (m_currentImage.save(fileName, "JPG", 90)) {
            QMessageBox::information(this, "保存成功",
                QString("截图已成功保存到:\n%1").arg(fileName));
        } else {
            QMessageBox::warning(this, "保存失败",
                QString("无法保存截图到:\n%1").arg(fileName));
        }
    }
}
