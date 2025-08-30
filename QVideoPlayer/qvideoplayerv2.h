#ifndef QVIDEOPLAYERV2_H
#define QVIDEOPLAYERV2_H

#include <QMainWindow>
#include <QMediaPlayer>
#include <QMediaPlaylist>
#include <QVideoWidget>

namespace Ui {
class QVideoPlayerV2;
}

class QVideoPlayerV2 : public QMainWindow
{
    Q_OBJECT

public:
    explicit QVideoPlayerV2(QWidget *parent = nullptr);
    ~QVideoPlayerV2();
    virtual bool eventFilter(QObject *watched, QEvent *event);
private slots:
    void on_listWidget_doubleClicked(const QModelIndex &index);

    void on_pushButton_clear_clicked();

    void on_pushButton_del_clicked();

    void on_pushButton_add_clicked();

    void on_pushButton_fullscreen_clicked();

    void on_pushButton_next_clicked();

    void on_pushButton_last_clicked();

    void on_pushButton_stop_clicked();

    void on_pushButton_pause_clicked();

    void on_pushButton_play_clicked();

    void on_verticalSlider_vol_valueChanged(int value);

    void on_horizontalSlider_progress_sliderMoved(int position);
    //周期改变槽函数
    void set_sider(qint64 duration);
    //进度改变
    void set_value(qint64 position);

private:
    Ui::QVideoPlayerV2 *ui;
    QMediaPlayer *player;
    QMediaPlaylist *playerList;
    QString video_duration;
    QVideoWidget *videoWidget;
};

#endif // QVIDEOPLAYERV2_H
