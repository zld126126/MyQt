#ifndef QVIDEOPLAYER_H
#define QVIDEOPLAYER_H

#include <QMainWindow>
#include <QMediaPlayer>
#include <QVideoWidget>

namespace Ui {
class QVideoPlayer;
}

class QVideoPlayer : public QMainWindow
{
    Q_OBJECT

public:
    explicit QVideoPlayer(QWidget *parent = nullptr);
    ~QVideoPlayer();

private slots:
    void on_pushButton_stop_clicked();

    void on_pushButton_play_clicked();

private:
    Ui::QVideoPlayer *ui;
    QMediaPlayer *player;
    QVideoWidget *widget;
};

#endif // QVIDEOPLAYER_H
