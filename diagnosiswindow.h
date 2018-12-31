#ifndef DIAGNOSISWINDOW_H
#define DIAGNOSISWINDOW_H

#include <QMainWindow>
#include "dbManager.h"
#include <QMessageBox>
#include <QSqlQuery>
#include <QDateTime>
#include <QTimer>
#include "medicinedialog.h"
#include "ui_diagnosiswindow.h"


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
signals:
    void setPspId(QString);

private slots:
    void getAccountAndToolTip(int account,int dpmdetail);
    void showStatistics();
    void on_AddButton_clicked();
    void on_DelButton_clicked();
    void on_OKButton_clicked();
    void on_PrintButton_clicked();
    void getMedicineInformation(QString,QString,QString,float,QString);
    void on_historyDiseaseListWidget_itemDoubleClicked(QListWidgetItem *item);
};

#endif // DIAGNOSISWINDOW_H
