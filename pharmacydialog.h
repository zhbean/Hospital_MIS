#ifndef PHARMACYDIALOG_H
#define PHARMACYDIALOG_H
#include<QDebug>
#include<QMessageBox>
#include <QSqlDatabase>
#include <QDialog>

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

private:
    Ui::PharmacyDialog *ui;
};

#endif // PHARMACYDIALOG_H
