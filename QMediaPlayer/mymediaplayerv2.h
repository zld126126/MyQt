#ifndef MYMEDIAPLAYERV2_H
#define MYMEDIAPLAYERV2_H

#include <QMainWindow>
#include <QMediaPlayer>
#include <QMediaPlaylist>

namespace Ui {
class MyMediaPlayerV2;
}

class MyMediaPlayerV2 : public QMainWindow
{
    Q_OBJECT

public:
    explicit MyMediaPlayerV2(QWidget *parent = nullptr);
    ~MyMediaPlayerV2();

private slots:
    void on_pushButton_play_clicked();

    void on_pushButton_pause_clicked();

    void on_pushButton_stop_clicked();

    void on_pushButton_add_clicked();

    void on_pushButton_del_clicked();

    void on_pushButton_clear_clicked();

    void on_pushButton_next_clicked();

    void on_pushButton_last_clicked();

    void on_listWidget_doubleClicked(const QModelIndex &index);

    void on_verticalSlider_vol_valueChanged(int value);

    void on_horizontalSlider_progress_sliderMoved(int position);

    //周期改变槽函数
    void set_sider(qint64 duration);
    //进度改变
    void set_value(qint64 position);

private:
    Ui::MyMediaPlayerV2 *ui;
    QMediaPlayer *player;
    QMediaPlaylist *playerList;
    QString music_duration;
};

#endif // MYMEDIAPLAYERV2_H
