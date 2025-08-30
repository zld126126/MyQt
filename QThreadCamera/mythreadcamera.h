#ifndef MYTHREADCAMERA_H
#define MYTHREADCAMERA_H

#include <QMainWindow>
#include <QLabel>
#include <QImage>
#include <QThread>
#include <QMutex>

QT_BEGIN_NAMESPACE
namespace Ui { class MyThreadCamera; }
QT_END_NAMESPACE

// 摄像头线程类，用于在后台执行摄像头初始化和帧捕获
class CameraThread : public QThread
{
    Q_OBJECT

public:
    CameraThread(QObject *parent = nullptr);
    ~CameraThread();
    
    void stop();
    unsigned char* getRGBData() const;
    unsigned int getRGBDataLen() const;
    bool isRunning() const { return m_isRunning; }

signals:
    void frameCaptured();      // 帧捕获完成信号
    void initFinished(bool success);  // 初始化完成信号

protected:
    void run() override;

private:
    bool m_isRunning;          // 线程运行状态
    mutable QMutex m_mutex;    // 用于保护数据访问的互斥锁
    unsigned char* m_rgbData;  // RGB数据指针
    unsigned int m_rgbDataLen; // RGB数据长度
}; 

class MyThreadCamera : public QMainWindow
{
    Q_OBJECT

public:
    MyThreadCamera(QWidget *parent = nullptr);
    ~MyThreadCamera();

private slots:
    void on_pushButton_start_clicked();
    void on_pushButton_stop_clicked();
    void onFrameCaptured();     // 处理帧捕获完成的槽函数
    void onInitFinished(bool success); // 处理初始化完成的槽函数

private:
    Ui::MyThreadCamera *ui;
    QLabel *m_cameraLabel;
    CameraThread *m_cameraThread;  // 替换原来的QTimer
    bool m_isCapturing;       // 标记是否正在捕获
    bool m_initSuccess;       // 标记摄像头是否初始化成功
    QImage m_currentImage;    // 保存当前帧用于截图

    void stopCapture();       // 停止捕获
    void saveScreenshot();    // 保存截图
};
#endif // MYTHREADCAMERA_H
