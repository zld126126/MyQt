#include "myprocesscamera.h"

#include <QApplication>

/*
    在linux环境执行:
    qmake
    make
    ./xxx
*/

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MyProcessCamera w;
    w.show();
    return a.exec();
}
