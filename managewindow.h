#ifndef MANAGEWINDOW_H
#define MANAGEWINDOW_H

#include <QMainWindow>
#include "dbManager.h"
#include "QStandardItemModel"
#include "QStandardItem"
#include<QMessageBox>

namespace Ui {
class manageWindow;
}

class manageWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit manageWindow(QWidget *parent = nullptr);
    ~manageWindow();

private:
    Ui::manageWindow *ui;
    void showEvent(QShowEvent *event);
    void initDepartment();

private slots:
    void getAccountAndToolTip(int account,int dpmdetail);
    void showSetting();
    void showStatistics();
    void on_trv_department_clicked(const QModelIndex &index);
    void on_btn_addDpm_clicked();
    void on_btn_addDpm_2_clicked();
    void on_btn_addDtl_clicked();
    void on_btn_updateDpm_2_clicked();
    void on_btn_delDpm_2_clicked();
    void on_btn_updateDpm_clicked();
    void on_btn_delDpm_clicked();
};

#endif // MANAGEWINDOW_H
