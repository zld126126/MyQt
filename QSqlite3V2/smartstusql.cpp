#include "smartstusql.h"
#include "ui_smartstusql.h"
#include <QDebug>
#include <QSqlQuery>
#include <QMessageBox>

SmartStuSQL::SmartStuSQL(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::SmartStuSQL)
{
    ui->setupUi(this);

    //添加数据库驱动
    db = QSqlDatabase::addDatabase("QSQLITE");
    //指定数据库的路径
    db.setDatabaseName("stu.db");
    //打开数据库
    if(!db.open()){
        qDebug()<<"打开数据库失败";
    }

    //初始化模型
    model = new QSqlTableModel(this);
}

SmartStuSQL::~SmartStuSQL()
{
    db.close();
    delete ui;
}

//建表
void SmartStuSQL::on_pushButton_create_clicked()
{
    QString sql = QString("create table if not exists %1(id int unique,name ntext,age int)").
            arg(ui->lineEdit_tablename->text());

    //执行sql语句
    QSqlQuery query;
    if(!query.exec(sql)){
        qDebug()<<"建表失败";
    }

    //绑定表格
    model->setTable(ui->lineEdit_tablename->text());
    //设置提交后才生效
    model->setEditStrategy(QSqlTableModel::OnManualSubmit);
    //显示
    model->select();
    ui->tableView->setModel(model);

    //禁止创建按钮和表名输入框
    ui->lineEdit_tablename->setEnabled(false);
    ui->pushButton_create->setEnabled(false);
}

//添加
void SmartStuSQL::on_pushButton_add_clicked()
{
    //获取当前行数
    int row = model->rowCount();
    //在最后一行之后插入
    model->insertRow(row);

    int id = 100;
    model->setData(model->index(row,0),id);

    //提交
    model->submitAll();
}

//保存
void SmartStuSQL::on_pushButton_save_clicked()
{
    model->submitAll();
}

//撤销
void SmartStuSQL::on_pushButton_revert_clicked()
{
    model->revertAll();
}

//查询
void SmartStuSQL::on_pushButton_select_clicked()
{
    model->setTable(ui->lineEdit_tablename->text());
    model->select();
}

//删除
void SmartStuSQL::on_pushButton_remove_clicked()
{
    //从表格获取要删除的行
    int delrow = ui->tableView->currentIndex().row();
    //删除改行
    model->removeRow(delrow);

    //询问用户是否确定删除
    if(QMessageBox::No==QMessageBox::warning(this,"删除","确定删除本行吗?",QMessageBox::Yes|QMessageBox::No)){
        model->revertAll();
    }
    else{
        model->submitAll();
    }
}

