#ifndef STUSQL_H
#define STUSQL_H

#include <QWidget>
#include <QStandardItemModel>
#include <QSqlDatabase>
#include <QSqlQuery>

QT_BEGIN_NAMESPACE
namespace Ui { class StuSQL; }
QT_END_NAMESPACE

class StuSQL : public QWidget
{
    Q_OBJECT

public:
    StuSQL(QWidget *parent = nullptr);
    ~StuSQL();

private slots:
    void on_pushButton_create_clicked();

    void on_pushButton_insert_clicked();

    void on_pushButton_select_clicked();

    void on_pushButton_delete_clicked();

    void on_pushButton_update_clicked();

private:
    Ui::StuSQL *ui;
    QStandardItemModel *model;
    quint16 count;//查询到的第几个数据
    QSqlDatabase db;
};
#endif // STUSQL_H
