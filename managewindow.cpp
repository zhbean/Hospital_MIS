#include "managewindow.h"
#include "ui_managewindow.h"

manageWindow::manageWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::manageWindow)
{
    ui->setupUi(this);
}

manageWindow::~manageWindow()
{
    delete ui;
}
void manageWindow::getAccountAndToolTip(int account,int dpmdetail)//account为账号即员工号，dpmdetail为科室详情号
{

}
