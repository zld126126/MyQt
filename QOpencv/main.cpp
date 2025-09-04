#include "mainwindow.h"

#include <QApplication>
#include <opencv.hpp>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgcodecs.hpp>
using namespace cv;

//opencv3.x版本
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    //展示一张图片
    Mat myimage = imread("D:\\QtProjects\\QOpencv\\1.jpg");
    imshow("Image", myimage);
    //等待10秒钟
    waitKey(10);

    MainWindow w;
    w.show();
    return a.exec();
}
