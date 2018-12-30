#ifndef PATIENTDIALOG_H
#define PATIENTDIALOG_H

#include <QDialog>
#include <QRegExp>
#include <QRegExpValidator>
namespace Ui {
class patientDialog;
}

class patientDialog : public QDialog
{
    Q_OBJECT

public:
    explicit patientDialog(QWidget *parent = nullptr);
    ~patientDialog();

private:
    Ui::patientDialog *ui;
    QString ID;
public:
    QString getId();
private slots:
    void on_buttonBox_accepted();
    void on_IDLineEdit_textChanged(const QString &arg1);
};

#endif // PATIENTDIALOG_H
