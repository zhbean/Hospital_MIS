#ifndef LOGIN_H
#define LOGIN_H

#include <QDialog>
#include <QDateTime>
#include <QTimer>
#include "dbManager.h"
#include "QStandardItem"
#include "QStandardItemModel"
#include "QMessageBox"

#include "qdebug.h"
#include "managewindow.h"
#include "diagnosiswindow.h"
#include "registerwindow.h"
#include "pharmacydialog.h"

namespace Ui {
class Login;
}

class Login : public QDialog
{
    Q_OBJECT

public:
    explicit Login(QWidget *parent = nullptr);
    ~Login();
    int limit=-1;//登陆权限
    bool judge=0;
private:
    Ui::Login *ui;
    void showEvent(QShowEvent *event);
    void checkDuty(int dpmdetail,int account);
    void writeDuty(int dutyID,int status,int staffID,QDateTime curTime);
public slots:
    void timerUpdate(void);
private slots:
    void on_OkButton_clicked();
    void on_CancelButton_clicked();
 signals:
    void setAccountAndToolTip(int,int);
};

#endif // LOGIN_H
