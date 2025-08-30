#include "studb.h"
#include "stusql.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    StuDB w1;
    w1.show();

    StuSQL w2;
    w2.show();
    return a.exec();
}
