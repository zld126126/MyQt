#include "stusql.h"
#include "ui_stusql.h"
#include <QDebug>

StuSQL::StuSQL(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::StuSQL)
{
    ui->setupUi(this);

    //初始化模型
    model = new QStandardItemModel(this);
    model->setHorizontalHeaderItem(0,new QStandardItem("id"));
    model->setHorizontalHeaderItem(1,new QStandardItem("name"));
    model->setHorizontalHeaderItem(2,new QStandardItem("age"));
    //绑定模型和表格
    ui->tableView->setModel(model);

    //添加数据库驱动
    db = QSqlDatabase::addDatabase("QSQLITE");
    //指定数据库的路径
    db.setDatabaseName("teacher.db");
    //打开数据库
    if(!db.open()){
        qDebug()<<"打开数据库失败";
    }
}

StuSQL::~StuSQL()
{
    db.close();
    delete ui;
}

//创建
void StuSQL::on_pushButton_create_clicked()
{
    QString sql = QString("create table if not exists %1(id int unique,name ntext,age int)").
            arg(ui->lineEdit_tablename->text());

    //执行sql语句
    QSqlQuery query;
    if(!query.exec(sql)){
        qDebug()<<"建表失败";
    }

    //禁止创建按钮和表名输入框
    ui->lineEdit_tablename->setEnabled(false);
    ui->pushButton_create->setEnabled(false);
}

//插入
void StuSQL::on_pushButton_insert_clicked()
{
    QString sql = QString("insert into %1 values(%2,'%3',%4)").
            arg(ui->lineEdit_tablename->text()).arg(ui->spinBox_insertid->value()).
            arg(ui->lineEdit_insertname->text()).arg(ui->spinBox_insertage->value());

    //执行sql语句
    QSqlQuery query;
    if(!query.exec(sql)){
        qDebug()<<"插入失败";
    }

    //刷新显示
    on_pushButton_select_clicked();
}

//查询
void StuSQL::on_pushButton_select_clicked()
{
    QString sql = QString("select * from %1;").arg(ui->lineEdit_tablename->text());

    count = 0;

    //删除之前显示
    this->model->removeRows(0,this->model->rowCount());

    QSqlQuery query;
    if(!query.exec(sql)){
        qDebug()<<"查询失败";
    }

    //获取查询结果
    while(query.next()){
        model->setItem(count,0,new QStandardItem(query.value(0).toString()));
        model->setItem(count,1,new QStandardItem(query.value(1).toString()));
        model->setItem(count,2,new QStandardItem(query.value(2).toString()));
        count++;
    }
}

//删除
void StuSQL::on_pushButton_delete_clicked()
{
    QString sql = QString("delete from %1 where id=%2").
            arg(ui->lineEdit_tablename->text()).arg(ui->spinBox_deleteid->value());

    //执行sql语句
    QSqlQuery query;
    if(!query.exec(sql)){
        qDebug()<<"删除失败";
    }

    //刷新显示
    on_pushButton_select_clicked();
}

//修改
void StuSQL::on_pushButton_update_clicked()
{
    QString sql;

    //修改ID
    if(ui->radioButton_newid->isChecked()){
        sql = QString("update %1 set id=%2 where id=%3").
                arg(ui->lineEdit_tablename->text()).arg(ui->spinBox_newid->value()).
                arg(ui->spinBox_updateid->value());
    }
    else if(ui->radioButton_newname->isChecked()){//修改name
        sql = QString("update %1 set name='%2' where id=%3").
                arg(ui->lineEdit_tablename->text()).arg(ui->lineEdit_newname->text()).
                arg(ui->spinBox_updateid->value());
    }
    else if(ui->radioButton_newage->isChecked()){//修改age
        sql = QString("update %1 set age=%2 where id=%3").
                arg(ui->lineEdit_tablename->text()).arg(ui->spinBox_newage->value()).
                arg(ui->spinBox_updateid->value());
    }

    //执行sql语句
    QSqlQuery query;
    if(!query.exec(sql)){
        qDebug()<<"删除失败";
    }

    //刷新显示
    on_pushButton_select_clicked();
}
