#include "mythreadcamera.h"
#include "ui_mythreadcamera.h"
#include <QImage>
#include <QPixmap>
#include <QDebug>
#include <QFileDialog>
#include <QDateTime>
#include <QMessageBox>

// 声明C接口
extern "C" {
    int camera_init();          // 初始化摄像头
    int camera_capture();       // 捕获一帧（生成RGB数据）
    unsigned char* get_rgb_data(); // 获取RGB数据指针
    unsigned int get_rgb_data_len(); // 获取RGB数据长度
    void free_rgb_data();       // 释放RGB数据
    void camera_cleanup();      // 清理摄像头资源
}

MyThreadCamera::MyThreadCamera(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MyThreadCamera)
    , m_isCapturing(false)
{
    ui->setupUi(this);

    m_cameraLabel = ui->label;
    m_cameraLabel->setAlignment(Qt::AlignCenter);
    m_cameraLabel->setText("点击开始按钮启动摄像头...");

    // 初始化计时器
    m_captureTimer = new QTimer(this);
    m_captureTimer->setInterval(100); // 间隔100ms，约10fps
    connect(m_captureTimer, &QTimer::timeout, this, &MyThreadCamera::onCaptureFrame);

    // 初始化摄像头（但不立即开始捕获）
    m_initSuccess = (camera_init() == 0);
    if (!m_initSuccess) {
        m_cameraLabel->setText("摄像头初始化失败！");
        ui->pushButton_start->setEnabled(false);
        qCritical() << "Camera init failed";
    }
}

MyThreadCamera::~MyThreadCamera()
{
    stopCapture();

    // 清理资源
    free_rgb_data();
    camera_cleanup();
    delete m_captureTimer;
    delete ui;
}

// 定时捕获图像的槽函数
void MyThreadCamera::onCaptureFrame()
{
    // 调用C接口捕获一帧（生成RGB数据）
    int captureRet = camera_capture();
    if (captureRet != 0) {
        qWarning() << "Capture frame failed, code:" << captureRet;
        return;
    }

    // 获取RGB数据（调用C接口）
    unsigned char *rgbData = get_rgb_data();
    unsigned int rgbLen = get_rgb_data_len();
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

// 开始捕获按钮点击事件
void MyThreadCamera::on_pushButton_start_clicked()
{
    if (!m_initSuccess) {
        QMessageBox::warning(this, "初始化失败", "摄像头初始化失败，无法启动");
        return;
    }

    if (!m_isCapturing) {
        m_captureTimer->start();
        m_isCapturing = true;
        ui->pushButton_start->setText("正在捕获...");
        ui->pushButton_start->setEnabled(false);
        ui->pushButton_stop->setEnabled(true);
        m_cameraLabel->setText("正在显示摄像头画面...");
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
        m_captureTimer->stop();
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
