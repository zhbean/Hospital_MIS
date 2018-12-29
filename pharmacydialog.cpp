#include "pharmacydialog.h"
#include "ui_pharmacydialog.h"

PharmacyDialog::PharmacyDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PharmacyDialog)
{
    ui->setupUi(this);
}

PharmacyDialog::~PharmacyDialog()
{
    delete ui;
}
void PharmacyDialog::getAccountAndToolTip(int account,int dpmdetail)//account为账号即员工号，dpmdetail为科室详情号
{

}
