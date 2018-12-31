#ifndef EDITDUTY_H
#define EDITDUTY_H

#include <QDialog>
#include "dbManager.h"
#include "QStandardItemModel"
#include "QStandardItem"
#include "QStringListModel"
#include "QCompleter"
#include "QMessageBox"
#include "QRegExp"

namespace Ui {
class EditDuty;
}

class EditDuty : public QDialog
{
    Q_OBJECT

public:
    explicit EditDuty(QWidget *parent = 0);
    ~EditDuty();
    void initCombobox();
    void initLineEdit();
    void initDate();
    void setPage(int flag);
    void initDuty(QString dutyID);

private slots:
    void on_btn_cAddDuty_clicked();

    void on_btn_cUpdateDuty_clicked();

    void on_btn_cDelDuty_clicked();

private:
    Ui::EditDuty *ui;
    void showEvent(QShowEvent * event);
};

#endif // EDITDUTY_H
