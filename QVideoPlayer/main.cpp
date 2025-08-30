#include "qvideoplayer.h"
#include "qvideoplayerv2.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    // 初始化资源（放在main函数之前或类定义之外）
    Q_INIT_RESOURCE(resources);

    QVideoPlayer w;
    w.show();

    QVideoPlayerV2 w2;
    w2.show();
    return a.exec();
}
