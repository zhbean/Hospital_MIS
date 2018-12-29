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
    void on_pushButton_selectDrug_clicked();
    void on_pushButton_purchase_clicked();
    void getAccountAndToolTip(int account,int dpmdetail);

private:
    Ui::PharmacyDialog *ui;
    int staff_id;




};

#endif // PHARMACYDIALOG_H
