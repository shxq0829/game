#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTcpSocket>
#include <QHostAddress>
#include <QMessageBox>
#include <QTime>

#include "settingdialog.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
public slots:
    void slt_setIPandPort(const QString &v_ip, int v_port, const QString &v_nameClient);
signals:
    void sig_makeSomeOneClose();
protected:
    void closeEvent(QCloseEvent *e);
private slots:
    void slt_sendMessage();
    void slt_reciveMessage();
    void slt_NetworkError();
    void slt_connect();
    void slt_about();
    void on_pushButton_2_clicked();

    void on_scissor_clicked();

    void on_paper_pressed();

    void on_rock_clicked();

private:
    Ui::MainWindow *ui;

    QString ip;
    int port;
    QTcpSocket c_socket;

    bool quitNormal;
};

#endif // MAINWINDOW_H
