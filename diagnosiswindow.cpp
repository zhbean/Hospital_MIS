#include "diagnosiswindow.h"
#include "ui_diagnosiswindow.h"

diagnosisWindow::diagnosisWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::diagnosisWindow)
{
    ui->setupUi(this);
}

diagnosisWindow::~diagnosisWindow()
{
    delete ui;
}
void diagnosisWindow::getAccountAndToolTip(int account,int dpmdetail) //account为账号即员工号，dpmdetail为科室详情号
{
    ui->patientIdLabel->setNum(account);
    ui->patientNameLabel->setNum(dpmdetail);
}
