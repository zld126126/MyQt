#ifndef MYCAMERA_H
#define MYCAMERA_H

#include <QMainWindow>
#include <QCamera>
#include <QCameraInfo>
#include <QCameraViewfinder>
#include <QCameraImageCapture>

namespace Ui {
class MyCamera;
}

class MyCamera : public QMainWindow
{
    Q_OBJECT

public:
    explicit MyCamera(QWidget *parent = nullptr);
    ~MyCamera();
    //生成截图名称
    QString generateCaptureFileName();

private slots:
    void on_pushButton_stop_clicked();

    void on_pushButton_start_clicked();

    void on_pushButton_cap_clicked();

    void set_pix(int id, const QImage &preview);

private:
    Ui::MyCamera *ui;
    QCamera *camera;
    QCameraViewfinder *cameraViewfinder;
    QCameraImageCapture *cameraImageCapture;
    QString currentCaptureFileName;
};

#endif // MYCAMERA_H
