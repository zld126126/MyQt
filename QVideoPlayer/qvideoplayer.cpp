#include "qvideoplayer.h"
#include "ui_qvideoplayer.h"
#include <QUrl>

QVideoPlayer::QVideoPlayer(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::QVideoPlayer)
{
    ui->setupUi(this);

    player = new QMediaPlayer(this);
    widget = new QVideoWidget(this);

    //播放窗口加入布局管理器
    ui->horizontalLayout->addWidget(widget);
    player->setVideoOutput(widget);
}

QVideoPlayer::~QVideoPlayer()
{
    delete ui;
}

void QVideoPlayer::on_pushButton_stop_clicked()
{
    player->stop();
}

void QVideoPlayer::on_pushButton_play_clicked()
{
    //player->setMedia(QUrl::fromLocalFile("D:\\QtProjects\\QVideoPlayer\\1.avi"));
    player->setMedia(QUrl("qrc:/1.avi"));
    player->play();
}

