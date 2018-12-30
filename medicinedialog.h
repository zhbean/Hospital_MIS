#ifndef MEDICINEDIALOG_H
#define MEDICINEDIALOG_H

#include <QDialog>
#include <QRegExp>
#include <QRegExpValidator>
#include "dbManager.h"
#include "QSqlQuery"
#include "infodialog.h"


namespace Ui {
class medicineDialog;
}

class medicineDialog : public QDialog
{
    Q_OBJECT

public:
    explicit medicineDialog(QWidget *parent = nullptr);
    ~medicineDialog();

private slots:
    void on_lineEdit_textChanged(const QString &arg1);

    void on_tableWidget_cellDoubleClicked(int row, int column);

private:
    Ui::medicineDialog *ui;

signals:
    void setMedicineInformation(QString,QString,QString,QString);
};

#endif // MEDICINEDIALOG_H
