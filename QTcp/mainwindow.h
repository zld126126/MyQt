#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_radioButton_2_clicked(bool checked);
    void on_radioButton_clicked(bool checked);
    void on_pushButton_3_clicked();
    //处理新的链接请求
    void accept_connect();
    //读取数据
    void read_date();
    void on_pushButton_5_clicked();
    void on_radioButton_clicked();

    void on_radioButton_3_clicked(bool checked);

private:
    Ui::MainWindow *ui;
};


#endif // MAINWINDOW_H
