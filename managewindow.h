#ifndef MANAGEWINDOW_H
#define MANAGEWINDOW_H

#include <QMainWindow>
#include "dbManager.h"
#include "QStandardItemModel"
#include "QStandardItem"
#include<QMessageBox>
#include "QSqlRelationalTableModel"
#include "QSqlQueryModel"
#include "editduty.h"
#include "QDateTime"
#include "showchart.h"
#include "login.h"

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
    int loginAccount;
    void showEvent(QShowEvent *event);
    void initDepartment();
    void initStaff();
    void initPosition();
    void initStaffCombobox();
    void initDuty();
    void initDpmStatistics();
    void initDilyStatistics();
    void initDutyRecord();

private slots:
    void getAccountAndToolTip(int account,int dpmdetail);
    void showSetting();
    void showStatistics();
    void signout();
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
    void on_btn_addStaff_clicked();
    void on_btn_updateStaff_clicked();
    void on_btn_delStaff_clicked();
    void on_trv_duty_clicked(const QModelIndex &index);
    void on_btn_addDuty_clicked();
    void on_btn_updateDuty_clicked();
    void on_btn_delDuty_clicked();
    void on_tabWidget_2_currentChanged(int index);
    void on_btn_dpmStatistics_clicked();
    void on_btn_dailyStatistics_clicked();
    void on_btn_charts_clicked();
};

#endif // MANAGEWINDOW_H
