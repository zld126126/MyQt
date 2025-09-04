#include "mainwindow.h"
#include <QApplication>
#include <opencv.hpp>
using namespace cv;

//opencv4.x版本
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    //展示一张图片
    Mat myimage = imread("D:\\QtProjects\\QOpencvV2\\1.jpg");
    namedWindow("OpenCV Test", WINDOW_NORMAL);
    imshow("OpenCV Test", myimage);
    //等待10秒钟
    waitKey(10);

    MainWindow w;
    w.show();
    return a.exec();
}
