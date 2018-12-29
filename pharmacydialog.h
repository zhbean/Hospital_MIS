#ifndef PHARMACYDIALOG_H
#define PHARMACYDIALOG_H

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

private:
    Ui::PharmacyDialog *ui;

private slots:
    void getAccountAndToolTip(int account,int dpmdetail);
};

#endif // PHARMACYDIALOG_H
