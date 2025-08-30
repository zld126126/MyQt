#include "qvideoplayerv2.h"
#include "ui_qvideoplayerv2.h"
#include <QFileDialog>
#include <QKeyEvent>

QVideoPlayerV2::QVideoPlayerV2(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::QVideoPlayerV2)
{
    ui->setupUi(this);

    player = new QMediaPlayer(this);
    playerList = new QMediaPlaylist(this);
    videoWidget = new QVideoWidget(this);

    //安装过滤器
    videoWidget->installEventFilter(this);

    //将播放窗口加入布局管理器
    ui->horizontalLayout->addWidget(videoWidget);
    player->setVideoOutput(videoWidget);

    playerList->setPlaybackMode(QMediaPlaylist::Loop);
    player->setPlaylist(playerList);

    //连接信号和槽
    QObject::connect(player,&QMediaPlayer::durationChanged,this,&QVideoPlayerV2::set_sider);
    QObject::connect(player,&QMediaPlayer::positionChanged,this,&QVideoPlayerV2::set_value);

    ui->pushButton_pause->setEnabled(false);
}

QVideoPlayerV2::~QVideoPlayerV2()
{
    delete ui;
}

void QVideoPlayerV2::on_pushButton_stop_clicked()
{
    qDebug() << "stop v2" << endl;
    player->stop();
    ui->pushButton_pause->setEnabled(false);
}

void QVideoPlayerV2::on_pushButton_pause_clicked()
{
    qDebug() << "pause v2" << endl;
    player->pause();
}

void QVideoPlayerV2::on_pushButton_play_clicked()
{
    qDebug() << "play v2" << endl;
    int row = ui->listWidget->currentRow();
    playerList->setCurrentIndex(row);
    player->play();
    player->setVolume(ui->verticalSlider_vol->value());

    ui->pushButton_pause->setEnabled(true);
}

void QVideoPlayerV2::on_listWidget_doubleClicked(const QModelIndex &index)
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

void QVideoPlayerV2::on_pushButton_clear_clicked()
{
    ui->listWidget->clear();
    playerList->clear();
    ui->pushButton_pause->setEnabled(false);
}

void QVideoPlayerV2::on_pushButton_del_clicked()
{
    int row = ui->listWidget->currentRow();
    QListWidgetItem *item = ui->listWidget->takeItem(row);
    if(item){
        delete item;
    }
    playerList->removeMedia(row);
}

void QVideoPlayerV2::on_pushButton_add_clicked()
{
    QStringList video_files = QFileDialog::getOpenFileNames(this, "选择视频文件", "qrc:/", "*.avi *.mp4");
    for (int i = 0; i < ui->listWidget->count() ; i++ ) {
        for (int j = 0; j < video_files.count() ; j++ ) {
            QString s1 = ui->listWidget->item(i)->text();
            QString s2 = video_files.at(j);
            if (s1 == s2){
                video_files.removeAt(j);
                break;
            }
        }
    }
    if (video_files.isEmpty()){
        qDebug() << "全部重复" << endl;
        return;
    }

    for (int i = 0; i < video_files.count() ; i++ ) {
        QString file = video_files.at(i);
        playerList->addMedia(QUrl::fromLocalFile(file));
    }
    ui->listWidget->addItems(video_files);
    ui->listWidget->setCurrentRow(0);
}

void QVideoPlayerV2::on_pushButton_fullscreen_clicked()
{
    videoWidget->setWindowFlags(Qt::Window);
    videoWidget->showFullScreen();
}

void QVideoPlayerV2::on_pushButton_next_clicked()
{
    int nextRow = 0;
    int row = playerList->currentIndex();
    if (row == playerList->mediaCount() - 1){
        nextRow = 0;
    }else{
        nextRow = row + 1;
    }
    playerList->setCurrentIndex(nextRow);
    player->play();
    player->setVolume(ui->verticalSlider_vol->value());
    ui->pushButton_pause->setEnabled(true);
    ui->listWidget->setCurrentRow(nextRow);
}

void QVideoPlayerV2::on_pushButton_last_clicked()
{
    int lastRow = 0;
    int row = playerList->currentIndex();
    if (row == 0){
        lastRow = playerList->mediaCount() - 1;
    }else{
        lastRow = row - 1;
    }
    playerList->setCurrentIndex(lastRow);
    player->play();
    player->setVolume(ui->verticalSlider_vol->value());
    ui->pushButton_pause->setEnabled(true);
    ui->listWidget->setCurrentRow(lastRow);
}

void QVideoPlayerV2::on_verticalSlider_vol_valueChanged(int value)
{
    player->setVolume(value);
    ui->label_vol->setText("音量:"+QString::number(value));
}

void QVideoPlayerV2::on_horizontalSlider_progress_sliderMoved(int position)
{
    player->setPosition(position);
}

void QVideoPlayerV2::set_sider(qint64 duration)
{
    ui->horizontalSlider_progress->setRange(0,duration);
    video_duration = QString("%1:%2").arg(duration/1000/60,2,10,QChar('0')).
                                      arg(duration/1000%60,2,10,QChar('0'));
    ui->label_progress->setText(QString("00:00/%1").arg(video_duration));
    ui->listWidget->setCurrentRow(playerList->currentIndex());
}

void QVideoPlayerV2::set_value(qint64 position)
{
    ui->horizontalSlider_progress->setValue(position);
    ui->label_progress->setText(QString("%1:%2/%3").arg(position/1000/60,2,10,QChar('0')).
                           arg(position/1000%60,2,10,QChar('0')).arg(video_duration));
}

bool QVideoPlayerV2::eventFilter(QObject *watched, QEvent *event)
{
    if(event->type() == QEvent::KeyPress){
        QKeyEvent *keyEvent = dynamic_cast<QKeyEvent *>(event);
        if(keyEvent->key() == Qt::Key_Escape){
            videoWidget->setWindowFlags(Qt::SubWindow);
            videoWidget->showNormal();
            videoWidget->setParent(this);
            ui->horizontalLayout->addWidget(videoWidget);
        }
    }
    return QWidget::eventFilter(watched, event);
}
