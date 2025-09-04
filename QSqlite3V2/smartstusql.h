#ifndef SMARTSTUSQL_H
#define SMARTSTUSQL_H

#include <QWidget>
#include <QSqlTableModel>
#include <QSqlDatabase>

QT_BEGIN_NAMESPACE
namespace Ui { class SmartStuSQL; }
QT_END_NAMESPACE

class SmartStuSQL : public QWidget
{
    Q_OBJECT

public:
    SmartStuSQL(QWidget *parent = nullptr);
    ~SmartStuSQL();

private slots:
    void on_pushButton_create_clicked();

    void on_pushButton_add_clicked();

    void on_pushButton_save_clicked();

    void on_pushButton_revert_clicked();

    void on_pushButton_select_clicked();

    void on_pushButton_remove_clicked();

private:
    Ui::SmartStuSQL *ui;
    QSqlTableModel *model;
    QSqlDatabase db;
};
#endif // SMARTSTUSQL_H
