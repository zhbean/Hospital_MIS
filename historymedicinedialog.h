#ifndef HISTORYMEDICINEDIALOG_H
#define HISTORYMEDICINEDIALOG_H

#include <QDialog>
#include <dbManager.h>
#include <QCheckBox>

namespace Ui {
class historyMedicineDialog;
}

class historyMedicineDialog : public QDialog
{
    Q_OBJECT

public:
    explicit historyMedicineDialog(QWidget *parent = nullptr);
    ~historyMedicineDialog();
    QStringList medicineId;
    QStringList medicineName;
    QStringList medicinenum;
    QStringList medicinePrice;
    QStringList medicineinfo;



private slots:
    void on_buttonBox_accepted();
    void getPspId(QString pspId);


private:
    Ui::historyMedicineDialog *ui;
};

#endif // HISTORYMEDICINEDIALOG_H
