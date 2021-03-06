#ifndef SETTINGDIALOG_H
#define SETTINGDIALOG_H

#include <QDialog>
#include <QHostAddress>
#include <QMessageBox>

namespace Ui {
class SettingDialog;
}

class SettingDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit SettingDialog(QWidget *parent = 0);
    ~SettingDialog();
signals:
    void sig_IpAndPort(const QString &ip, int port, const QString &nameClient);
private slots:
    void slt_doButton();

private:
    Ui::SettingDialog *ui;
};

#endif // SETTINGDIALOG_H
