#include "settingdialog.h"
#include "ui_settingdialog.h"
#include <iostream>

SettingDialog::SettingDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SettingDialog)
{
    ui->setupUi(this);
    connect(ui->pushButton,SIGNAL(clicked()),this,SLOT(slt_doButton()));
}

SettingDialog::~SettingDialog()
{
    delete ui;
}

void SettingDialog::slt_doButton()
{
    QHostAddress address;
    if(!address.setAddress(ui->lineEdit->text().trimmed())) {
        QMessageBox::information(this, tr("Info"), tr("IP Error!"));
        return;
    }
    bool ok = false;
    int port = ui->lineEdit_2->text().toInt(&ok);//port = 18090 success to transmit and ok change to TRUE
    QString clientName = ui->lineEdit_3->text().trimmed();
    //std::cout << clientName.toStdString()<<std::endl;
    if (ok) {
        emit sig_IpAndPort(ui->lineEdit->text(), port, clientName);
    }
}

