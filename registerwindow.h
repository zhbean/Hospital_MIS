#ifndef REGISTERWINDOW_H
#define REGISTERWINDOW_H

#include <QMainWindow>
#include "dbManager.h"
#include<QMessageBox>

namespace Ui {
class registerWindow;
}

class registerWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit registerWindow(QWidget *parent = nullptr);
    ~registerWindow();
    bool login=0;

private:
    Ui::registerWindow *ui;

private slots:
    void getAccountAndToolTip(int account,int dpmdetail);
    void on_registerPushButton_clicked();
    void on_searchPushButton_clicked();
    void on_PayPushButton_clicked();
    void on_IDLineEdit_textChanged(const QString &arg1);
};

#endif // REGISTERWINDOW_H
