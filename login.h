#ifndef LOGIN_H
#define LOGIN_H

#include <QDialog>
#include <QDateTime>
#include <QTimer>
#include "dbManager.h"
#include "QStandardItem"
#include "QStandardItemModel"
#include "QMessageBox"

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

private:
    Ui::Login *ui;
    void showEvent(QShowEvent *event);
public slots:
    void timerUpdate(void);
private slots:
    void on_OkButton_clicked();
    void on_CancelButton_clicked();
 signals:
    void setAccountAndToolTip(int,int);
};

#endif // LOGIN_H
