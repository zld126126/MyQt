#include "mymediaplayerv2.h"
#include "ui_mymediaplayerv2.h"
#include <QFileDialog>

MyMediaPlayerV2::MyMediaPlayerV2(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MyMediaPlayerV2)
{
    ui->setupUi(this);

    player = new QMediaPlayer(this);
    // 使用绝对路径测试（替换为你的实际文件路径）
    //player->setMedia(QUrl::fromLocalFile("D:/QtProjects/QMediaPlayer/2.mp3"));
    //player->setMedia(QMediaContent(QUrl("qrc:/2.mp3")));

    playerList = new QMediaPlaylist();
    playerList->setPlaybackMode(QMediaPlaylist::Loop);
    player->setPlaylist(playerList);

    //连接信号和槽
    QObject::connect(player,&QMediaPlayer::durationChanged,this,&MyMediaPlayerV2::set_sider);
    QObject::connect(player,&QMediaPlayer::positionChanged,this,&MyMediaPlayerV2::set_value);

    ui->pushButton_pause->setEnabled(false);
}

MyMediaPlayerV2::~MyMediaPlayerV2()
{
    delete ui;
}

void MyMediaPlayerV2::on_pushButton_play_clicked()
{
    qDebug() << "play v2" << endl;
    int row = ui->listWidget->currentRow();
    playerList->setCurrentIndex(row);
    player->play();
    player->setVolume(ui->verticalSlider_vol->value());

    ui->pushButton_pause->setEnabled(true);
}

void MyMediaPlayerV2::on_pushButton_pause_clicked()
{
    qDebug() << "pause v2" << endl;
    player->pause();
}

void MyMediaPlayerV2::on_pushButton_stop_clicked()
{
    qDebug() << "stop v2" << endl;
    player->stop();
    ui->pushButton_pause->setEnabled(false);
}

void MyMediaPlayerV2::on_pushButton_add_clicked()
{
    QStringList music_files = QFileDialog::getOpenFileNames(this, "选择音乐文件", "qrc:/", "*.mp3 *.wav");
    for (int i = 0; i < ui->listWidget->count() ; i++ ) {
        for (int j = 0; j < music_files.count() ; j++ ) {
            QString s1 = ui->listWidget->item(i)->text();
            QString s2 = music_files.at(j);
            if (s1 == s2){
                music_files.removeAt(j);
                break;
            }
        }
    }
    if (music_files.isEmpty()){
        qDebug() << "全部重复" << endl;
        return;
    }

    for (int i = 0; i < music_files.count() ; i++ ) {
        QString file = music_files.at(i);
        playerList->addMedia(QUrl::fromLocalFile(file));
    }
    ui->listWidget->addItems(music_files);
    ui->listWidget->setCurrentRow(0);
}

void MyMediaPlayerV2::on_pushButton_del_clicked()
{
    int row = ui->listWidget->currentRow();
    QListWidgetItem *item = ui->listWidget->takeItem(row);
    if(item){
        delete item;
    }
    playerList->removeMedia(row);
}

void MyMediaPlayerV2::on_pushButton_clear_clicked()
{
    ui->listWidget->clear();
    playerList->clear();
    ui->pushButton_pause->setEnabled(false);
}

void MyMediaPlayerV2::on_pushButton_next_clicked()
{
    int nextRow = 0;
    int row = playerList->currentIndex();
    if (row == playerList->mediaCount() - 1){
        nextRow = 0;
        playerList->setCurrentIndex(nextRow);
        player->play();
        player->setVolume(ui->verticalSlider_vol->value());
        ui->pushButton_pause->setEnabled(true);
        ui->listWidget->setCurrentRow(nextRow);
    }else{
        nextRow = row + 1;
        playerList->setCurrentIndex(nextRow);
        player->play();
        player->setVolume(ui->verticalSlider_vol->value());
        ui->pushButton_pause->setEnabled(true);
        ui->listWidget->setCurrentRow(nextRow);
    }
}

void MyMediaPlayerV2::on_pushButton_last_clicked()
{
    int lastRow = 0;
    int row = playerList->currentIndex();
    if (row == 0){
        lastRow = playerList->mediaCount() - 1;
        playerList->setCurrentIndex(lastRow);
        player->play();
        player->setVolume(ui->verticalSlider_vol->value());
        ui->pushButton_pause->setEnabled(true);
        ui->listWidget->setCurrentRow(lastRow);
    }else{
        lastRow = row - 1;
        playerList->setCurrentIndex(lastRow);
        player->play();
        player->setVolume(ui->verticalSlider_vol->value());
        ui->pushButton_pause->setEnabled(true);
        ui->listWidget->setCurrentRow(lastRow);
    }
}

void MyMediaPlayerV2::on_listWidget_doubleClicked(const QModelIndex &index)
{
    int row1 = playerList->currentIndex();
    qDebug() << "double clicked:" << row1 << endl;
    int row2 = index.row();
    // 判断双击的索引是否是当前选中的索引
    if (row1 == -1){

    }else{
        if (row1 == row2) {
            if (player->state() == QMediaPlayer::PausedState){
                //播放
                player->play();
                player->setVolume(ui->verticalSlider_vol->value());
                ui->pushButton_pause->setEnabled(true);
                return;
            }else{
                //暂停
                player->pause();
                return;
            }
        }
    }
    playerList->setCurrentIndex(row2);
    player->play();
    player->setVolume(ui->verticalSlider_vol->value());
    ui->pushButton_pause->setEnabled(true);
    ui->listWidget->setCurrentRow(row2);
}

void MyMediaPlayerV2::on_verticalSlider_vol_valueChanged(int value)
{
    player->setVolume(value);
    ui->label_vol->setText("音量:"+QString::number(value)+"%");
}

void MyMediaPlayerV2::set_sider(qint64 duration)
{
    ui->horizontalSlider_progress->setRange(0,duration);
    music_duration = QString("%1:%2").arg(duration/1000/60,2,10,QChar('0')).
                                      arg(duration/1000%60,2,10,QChar('0'));
    ui->label_progress->setText(QString("00:00/%1").arg(music_duration));
    ui->listWidget->setCurrentRow(playerList->currentIndex());
}

void MyMediaPlayerV2::set_value(qint64 position)
{
    ui->horizontalSlider_progress->setValue(position);
    ui->label_progress->setText(QString("%1:%2/%3").arg(position/1000/60,2,10,QChar('0')).
                           arg(position/1000%60,2,10,QChar('0')).arg(music_duration));
}

void MyMediaPlayerV2::on_horizontalSlider_progress_sliderMoved(int position)
{
    player->setPosition(position);
}

