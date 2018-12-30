#ifndef DIAGNOSISWINDOW_H
#define DIAGNOSISWINDOW_H

#include <QMainWindow>
#include "dbManager.h"
#include <QMessageBox>
#include <QSqlQuery>


namespace Ui {
class diagnosisWindow;
}

class diagnosisWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit diagnosisWindow(QWidget *parent = nullptr);
    ~diagnosisWindow();

private:
    Ui::diagnosisWindow *ui;

private slots:
    void getAccountAndToolTip(int account,int dpmdetail);
    void showStatistics();
};

#endif // DIAGNOSISWINDOW_H
