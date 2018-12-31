#ifndef PHARMACYDIALOG_H
#define PHARMACYDIALOG_H
#include<QDebug>
#include<QMessageBox>
#include <QSqlDatabase>
#include <QDialog>
#include <QDateTime>

namespace Ui {
class PharmacyDialog;
}

class PharmacyDialog : public QDialog
{
    Q_OBJECT

public:
    explicit PharmacyDialog(QWidget *parent = nullptr);
    ~PharmacyDialog();

private slots:

    //填充标题栏
    void getAccountAndToolTip(int account,int dpmdetail);
    //tab库存
    void on_pushButton_selectDrug_clicked();
    void on_pushButton_purchase_clicked();
    void on_pushButton_return_clicked();
    //tab发放药品
    void on_pushButton_tabSale_selectPayment_clicked();
    void on_tableWidget_sale_cellDoubleClicked(int row, int column);
    void on_pushButton_tabSale_grant_clicked();
    //tab操作记录
    void on_pushButton_selectByDrugId_clicked();
    void on_pushButton_selectByStaffId_clicked();
    void on_pushButton_recordTab_selectByDrugName_clicked();
    //tab出厂商
    void on_pushButton_addFactory_clicked();
    void on_pushButton_tabFactory_UpdateFactory_clicked();
    void on_pushButton_tabFactory_select_clicked();




private:
    Ui::PharmacyDialog *ui;
    int staff_id;//当前登陆的员工号




};

#endif // PHARMACYDIALOG_H
