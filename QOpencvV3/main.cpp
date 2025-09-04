#include "mainwindow.h"

#include <QApplication>
#include <opencv.hpp>
using namespace cv;

//opencv4.x版本
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    //展示一张图片
    Mat myimage = imread("1.jpg");
    imshow("Image", myimage);
    //等待10秒钟
    waitKey(10);

    MainWindow w;
    w.show();
    return a.exec();
}
