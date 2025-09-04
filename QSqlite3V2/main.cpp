#include "smartstusql.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    SmartStuSQL w;
    w.show();
    return a.exec();
}
