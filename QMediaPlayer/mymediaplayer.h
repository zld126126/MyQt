#ifndef MYMEDIAPLAYER_H
#define MYMEDIAPLAYER_H

#include <QMainWindow>
#include <QMediaPlayer>
#include <QMediaPlaylist>

namespace Ui {
class MyMediaPlayer;
}

class MyMediaPlayer : public QMainWindow
{
    Q_OBJECT

public:
    explicit MyMediaPlayer(QWidget *parent = nullptr);
    ~MyMediaPlayer();

private slots:
    void on_pushButton_pause_clicked();

    void on_pushButton_stop_clicked();

    void on_pushButton_play_clicked();

private:
    Ui::MyMediaPlayer *ui;
    QMediaPlayer *player;
};

#endif // MYMEDIAPLAYER_H
