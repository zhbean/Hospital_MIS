#include "updatepricedialog.h"
#include "ui_updatepricedialog.h"

updatePriceDialog::updatePriceDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::updatePriceDialog)
{
    ui->setupUi(this);
}

updatePriceDialog::~updatePriceDialog()
{
    delete ui;
}

void updatePriceDialog::on_pushButton_ok_clicked()
{
    MyDrugId=ui->lineEdit_drugId->text();
    MyDrugName=ui->lineEdit_drugName->text();
    MyDrugBuyPrice=ui->lineEdit_drugBuyPrice->text();
    MyDrugSalePrice=ui->lineEdit_drugSalePrice->text();
    this->accept();
}

void updatePriceDialog::on_pushButton_cancel_clicked()
{

    this->close();
}
//
void updatePriceDialog::getDrugMessage(QString drug_id,QString drug_name,QString drug_buyPrice,QString drug_Price)
{
    ui->lineEdit_drugId->setText(drug_id);
    ui->lineEdit_drugName->setText(drug_name);
    ui->lineEdit_drugBuyPrice->setText(drug_buyPrice);
    ui->lineEdit_drugSalePrice->setText(drug_Price);
    qDebug()<<"before emit ";
}
