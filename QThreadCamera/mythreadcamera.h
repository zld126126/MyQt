#ifndef MYTHREADCAMERA_H
#define MYTHREADCAMERA_H

#include <QMainWindow>
#include <QLabel>
#include <QTimer>
#include <QImage>

QT_BEGIN_NAMESPACE
namespace Ui { class MyThreadCamera; }
QT_END_NAMESPACE

class MyThreadCamera : public QMainWindow
{
    Q_OBJECT

public:
    MyThreadCamera(QWidget *parent = nullptr);
    ~MyThreadCamera();

private slots:
    void onCaptureFrame();
    void on_pushButton_start_clicked();
    void on_pushButton_stop_clicked();

private:
    Ui::MyThreadCamera *ui;
    QLabel *m_cameraLabel;
    QTimer *m_captureTimer;
    bool m_isCapturing;       // 标记是否正在捕获
    bool m_initSuccess;       // 标记摄像头是否初始化成功
    QImage m_currentImage;    // 保存当前帧用于截图

    void stopCapture();       // 停止捕获
    void saveScreenshot();    // 保存截图
};
#endif // MYTHREADCAMERA_H
