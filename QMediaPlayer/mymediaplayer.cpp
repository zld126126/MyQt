#include "mymediaplayer.h"
#include "ui_mymediaplayer.h"
#include <QDebug>
#include <QUrl>

MyMediaPlayer::MyMediaPlayer(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MyMediaPlayer)
{
    ui->setupUi(this);
    player = new QMediaPlayer(this);
    // 使用绝对路径测试（替换为你的实际文件路径）
    //player->setMedia(QUrl::fromLocalFile("D:/QtProjects/QMediaPlayer/2.mp3"));
    player->setMedia(QMediaContent(QUrl("qrc:/2.mp3")));
}

MyMediaPlayer::~MyMediaPlayer()
{
    delete ui;
}

void MyMediaPlayer::on_pushButton_pause_clicked()
{
    qDebug() << "pause" << endl;
    player->pause();
}

void MyMediaPlayer::on_pushButton_stop_clicked()
{
    qDebug() << "stop" << endl;
    player->stop();
}

void MyMediaPlayer::on_pushButton_play_clicked()
{
    qDebug() << "play" << endl;
    player->play();
}

