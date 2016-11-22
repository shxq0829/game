/********************************************************************
* author 周翔
* e-mail 604487178@qq.com
* blog http://blog.csdn.net/zhx6044
**********************************************************************/

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <iostream>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    quitNormal(false)
{
    ui->setupUi(this);

    ui->pushButton->setDefault(true);

    connect(ui->pushButton,SIGNAL(clicked()),this,SLOT(slt_sendMessage()));
    connect(&c_socket,SIGNAL(readyRead()),this,SLOT(slt_reciveMessage()));
    connect(&c_socket,SIGNAL(disconnected()),this,SLOT(slt_NetworkError()));
    connect(ui->action,SIGNAL(triggered()),this,SLOT(slt_connect()));
    connect(ui->action_3,SIGNAL(triggered()),this,SLOT(slt_about()));
    connect(ui->action_2,SIGNAL(triggered()),this,SLOT(close()));
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::closeEvent(QCloseEvent *e)
{
    quitNormal = true;
    if(c_socket.isOpen()) {
        c_socket.disconnectFromHost();
    }
    QMainWindow::closeEvent(e);
}

void MainWindow::slt_setIPandPort(const QString &v_ip, int v_port, const QString &v_nameClient)
{
    //std::cout << v_nameClient.toStdString() << std::endl;
    QHostAddress address;
    ip = v_ip;
    port = v_port;
    address.setAddress(ip);
    emit sig_makeSomeOneClose();//close Menu dialog
    c_socket.connectToHost(address,port);
    if (c_socket.waitForConnected(3000)) {
        ui->textEdit->append(v_nameClient+" connect successfily!");
        if(c_socket.state() == QTcpSocket::ConnectedState) {
            c_socket.write(v_nameClient.toLatin1());
            std::cout << "success to name" <<std::endl;
        } else { }
    } else {
        ui->textEdit->append(v_nameClient+" connect failly!" + c_socket.error());
    }
}

void MainWindow::slt_sendMessage()
{
    QString message = ui->textEdit_2->toPlainText();
    ui->textEdit_2->clear();
    if (c_socket.state() == QTcpSocket::ConnectedState) {
        c_socket.write(message.toLatin1());
        ui->textEdit->append(QTime::currentTime().toString("hh:mm:ss") +" "+ message);
    } else {

    }
}
void MainWindow::slt_reciveMessage()
{
    QByteArray message = c_socket.readAll();
    ui->textEdit->append(message);
}

void MainWindow::slt_NetworkError()
{
    if (!quitNormal)
        QMessageBox::warning(this, tr("ERROR"), tr("NetWork ERROR!"));
}
//dialog which is used for seting ip and port  Menu Items
void MainWindow::slt_connect()
{
    SettingDialog dialog(this);
    dialog.setModal(true);
    connect(&dialog,SIGNAL(sig_IpAndPort(QString,int,QString)),this,SLOT(slt_setIPandPort(QString,int,QString)));
    connect(this,SIGNAL(sig_makeSomeOneClose()),&dialog,SLOT(close()));
    dialog.show();
    dialog.exec();
}

void MainWindow::slt_about()
{
    QApplication::aboutQt();
}

void MainWindow::on_pushButton_2_clicked()
{
    QString ls = "info";
    if(c_socket.state() == QTcpSocket::ConnectedState) {
        c_socket.write(ls.toLatin1());
        std::cout << "success to info" <<std::endl;
    } else { }
}
