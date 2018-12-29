#ifndef DIAGNOSISWINDOW_H
#define DIAGNOSISWINDOW_H

#include <QMainWindow>

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
};

#endif // DIAGNOSISWINDOW_H
