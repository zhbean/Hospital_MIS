#ifndef PHARMACYDIALOG_H
#define PHARMACYDIALOG_H
#include<QDebug>
#include<QMessageBox>
#include <QSqlDatabase>
#include <QDialog>
#include <QDateTime>
#include "updatepricedialog.h"

namespace Ui {
class PharmacyDialog;
}

class PharmacyDialog : public QDialog
{
    Q_OBJECT

public:
    explicit PharmacyDialog(QWidget *parent = nullptr);
    ~PharmacyDialog();
signals:
    void sendDrugMessage(QString,QString,QString,QString);

private slots:

    //填充标题栏
    void getAccountAndToolTip(int account,int dpmdetail);
    //tab药品库存
    void on_pushButton_selectDrug_clicked();
    void on_pushButton_purchase_clicked();
    void on_pushButton_return_clicked();
    //修改药品价格
    void on_tableWidget_store_cellDoubleClicked(int row, int column);
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
    //关闭药房
    void on_pushButton_closeDoor_clicked();



    void on_tableWidget_factory_cellDoubleClicked(int row, int column);

private:
    Ui::PharmacyDialog *ui;
    int staff_id;//当前登陆的员工号
    updatePriceDialog* pUpdatePriceDialog;




};

#endif // PHARMACYDIALOG_H
