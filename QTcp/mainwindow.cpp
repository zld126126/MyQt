#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QHostAddress>
#include <QTcpServer>
#include <QTcpSocket>
#include <QMessageBox>
#include <QDebug>


QTcpServer *server;
QTcpSocket *socket;

QTcpSocket *new_socket;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    //分配空间
    server  =  new  QTcpServer(this);
    socket  =  new  QTcpSocket(this);

    connect(socket,&QTcpSocket::readyRead,this,&MainWindow::read_date);
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_radioButton_2_clicked(bool checked)
{
    if(checked)
    {
        ui->pushButton_3->setText("开始监听");
        //关联信号与槽
        connect(server,&QTcpServer::newConnection,this,&MainWindow::accept_connect);
    }
}

void MainWindow::on_radioButton_clicked(bool checked)
{
    if(checked)
    {
        ui->pushButton_3->setText("链接网络");
        //取消新的链接请求信号
        disconnect(server,&QTcpServer::newConnection,this,&MainWindow::accept_connect);
        //取消服务器的监听
        server->close();
    }
}


//链接网络与开始监听按钮
void MainWindow::on_pushButton_3_clicked()
{
    //判断是链接网络还是监听
    if(ui->pushButton_3->text() == "开始监听")
    {

        QString local_port = ui->lineEdit_prot1->text();
        //开始监听
        if(server->listen(QHostAddress::Any,local_port.toShort()))
        {
            ui->label_4->setText("正在监听");
        }else
        {
            ui->label_4->setText("监听失败");
        }
    }

    if(ui->pushButton_3->text() == "链接网络")
    {
        //获取端口和 IP
        QString ip =  ui->lineEdit_ip->text();
        QString port = ui->lineEdit_prot->text();

        //链接服务器
        socket->connectToHost(ip,port.toInt());
    }
}

//新的链接请求
void MainWindow::accept_connect()
{
    int ret = QMessageBox::warning(this, tr("链接提示"),
                                   tr("有新的客户端链接是否接收请求\n"),
                                   QMessageBox::Yes|QMessageBox::No);

    if(ret == QMessageBox::Yes)  //接收链接请求
    {
        new_socket  =  server->nextPendingConnection();

        qDebug() << new_socket->peerAddress().toString();
        //关联可读信号
        connect(new_socket,&QTcpSocket::readyRead,this,&MainWindow::read_date);
    }
}

void MainWindow::read_date()
{
    ui->plainTextEdit->appendPlainText(new_socket->readAll());
    ui->plainTextEdit->appendPlainText(socket->readAll());
}

void MainWindow::on_pushButton_5_clicked()
{
    if(ui->pushButton_3->text() == "开始监听")
    {
        QString msg  = ui->plainTextEdit_2->toPlainText();
        qDebug() <<  msg;
        new_socket->write(msg.toUtf8());
    }
    if(ui->pushButton_3->text() == "链接网络")
    {
        QString msg  = ui->plainTextEdit_2->toPlainText();
        qDebug() <<  msg;
        socket->write(msg.toUtf8());
    }
}

void MainWindow::on_radioButton_clicked()
{

}

void MainWindow::on_radioButton_3_clicked(bool checked)
{
    if(checked){
        ui->pushButton_3->setText("TODO");
        qDebug() << "TODO UDP" << endl;
    }
}

