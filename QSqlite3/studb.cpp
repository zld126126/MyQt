#include "studb.h"
#include "ui_studb.h"
#include <QDebug>

StuDB::StuDB(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::StuDB)
{
    ui->setupUi(this);

    //初始化模型
    model = new QStandardItemModel(this);
    model->setHorizontalHeaderItem(0,new QStandardItem("id"));
    model->setHorizontalHeaderItem(1,new QStandardItem("name"));
    model->setHorizontalHeaderItem(2,new QStandardItem("age"));
    //绑定模型和表格
    ui->tableView->setModel(model);

    //打开数据库
    int res = sqlite3_open("./stu.db",&pDb);
    if(res!=SQLITE_OK){
        qDebug()<<"打开失败";
    }

}

StuDB::~StuDB()
{
    delete ui;
    //关闭数据库
    sqlite3_close(pDb);
}

//建表
void StuDB::on_pushButton_create_clicked()
{
    QString sql = QString("create table if not exists %1(id int unique,name ntext,age int);").
            arg(ui->lineEdit_tablename->text());

    //执行sql语句

    int res = sqlite3_exec(pDb,sql.toStdString().c_str(),NULL,NULL,NULL);
    if(res!=SQLITE_OK){
        qDebug()<<"建表失败";
    }

    //禁止创建按钮和表名输入框
    ui->lineEdit_tablename->setEnabled(false);
    ui->pushButton_create->setEnabled(false);
}

//插入
void StuDB::on_pushButton_insert_clicked()
{
    QString sql = QString("insert into %1 values(%2,'%3',%4);").
            arg(ui->lineEdit_tablename->text()).arg(ui->spinBox_insertid->value()).
            arg(ui->lineEdit_insertname->text()).arg(ui->spinBox_insertage->value());

    //执行sql语句

    int res = sqlite3_exec(pDb,sql.toStdString().c_str(),NULL,NULL,NULL);
    if(res!=SQLITE_OK){
        qDebug()<<"插入失败";
    }

    //刷新更新
    on_pushButton_select_clicked();
}

//回调函数
static int stu_callback(void *arg,int col,char **str,char **name)
{
    //恢复对象
    StuDB *db = static_cast<StuDB *>(arg);

    for(int i=0;i<col;i++){
        //qDebug()<<name[i]<<":"<<str[i];
        //将查询到的对象显示到模型
        db->model->setItem(db->count,i,new QStandardItem(str[i]));
    }
    db->count++;

    return SQLITE_OK;
}

//查询
void StuDB::on_pushButton_select_clicked()
{
    QString sql = QString("select * from %1;").arg(ui->lineEdit_tablename->text());

    count = 0;
    //执行sql语句
    int res = sqlite3_exec(pDb,sql.toStdString().c_str(),stu_callback,this,NULL);
    if(res!=SQLITE_OK){
        qDebug()<<"查询失败";
    }
}

//删除
void StuDB::on_pushButton_delete_clicked()
{
    QString sql = QString("delete from %1 where id=%2;").
            arg(ui->lineEdit_tablename->text()).arg(ui->spinBox_deleteid->value());

    //执行sql语句
    int res = sqlite3_exec(pDb,sql.toStdString().c_str(),NULL,NULL,NULL);
    if(res!=SQLITE_OK){
        qDebug()<<"删除失败";
    }

    //删除所有行
    this->model->removeRows(0,this->model->rowCount());
    //刷新显示
    on_pushButton_select_clicked();
}

//更新
void StuDB::on_pushButton_update_clicked()
{
    QString sql;

    //修改ID
    if(ui->radioButton_newid->isChecked()){
        sql = QString("update %1 set id=%2 where id=%3;").
                arg(ui->lineEdit_tablename->text()).arg(ui->spinBox_newid->value()).
                arg(ui->spinBox_updateid->value());
    }
    else if(ui->radioButton_newname->isChecked()){//修改name
        sql = QString("update %1 set name='%2' where id=%3;").
                arg(ui->lineEdit_tablename->text()).arg(ui->lineEdit_newname->text()).
                arg(ui->spinBox_updateid->value());
    }
    else if(ui->radioButton_newage->isChecked()){//修改age
        sql = QString("update %1 set age=%2 where id=%3;").
                arg(ui->lineEdit_tablename->text()).arg(ui->spinBox_newage->value()).
                arg(ui->spinBox_updateid->value());
    }

    //执行sql语句
    int res = sqlite3_exec(pDb,sql.toStdString().c_str(),NULL,NULL,NULL);
    if(res!=SQLITE_OK){
        qDebug()<<"修改失败";
    }

    //刷新显示
    on_pushButton_select_clicked();
}
