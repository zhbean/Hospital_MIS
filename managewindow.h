#ifndef MANAGEWINDOW_H
#define MANAGEWINDOW_H

#include <QMainWindow>
#include "dbManager.h"
#include "QStandardItemModel"
#include "QStandardItem"
#include<QMessageBox>
#include "QSqlRelationalTableModel"
#include "QSqlQueryModel"

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
    void initStaff();
    void initPosition();
    void initStaffCombobox();

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
    void on_tabWidget_currentChanged(int index);
    void on_tbv_position_clicked(const QModelIndex &index);
    void on_btn_addPosition_clicked();
    void on_btn_updatePosition_clicked();
    void on_btn_delPosition_clicked();
    void on_tbv_staff_clicked(const QModelIndex &index);
};

#endif // MANAGEWINDOW_H
