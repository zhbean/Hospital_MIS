#include "registerwindow.h"
#include "ui_registerwindow.h"

registerWindow::registerWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::registerWindow)
{
    ui->setupUi(this);
}

registerWindow::~registerWindow()
{
    delete ui;
}
void registerWindow::getAccountAndToolTip(int account,int dpmdetail)//account为账号即员工号，dpmdetail为科室详情号
{

}
