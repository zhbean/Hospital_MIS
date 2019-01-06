#include "medicinedialog.h"
#include "ui_medicinedialog.h"
#include <QtDebug>

medicineDialog::medicineDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::medicineDialog)
{
    ui->setupUi(this);
    ui->tableWidget->setColumnWidth(0,100);
    ui->tableWidget->setColumnWidth(1,380);
    ui->tableWidget->setColumnWidth(2,90);
    ui->tableWidget->setColumnWidth(3,100);
    QRegExp rx("^[a-zA-Z0-9\u4e00-\u9fa5\\w]+$");
    ui->lineEdit->setValidator(new QRegExpValidator(rx,this));
}

medicineDialog::~medicineDialog()
{
    delete ui;

}

void medicineDialog::on_lineEdit_textChanged(const QString &arg1)
{
    if(!ui->lineEdit->text().isEmpty()){
        while (ui->tableWidget->rowCount()) {
            ui->tableWidget->removeRow(0);
        }
        QRegExp rx("\\d{0,}");
        QRegExpValidator v(rx, this);
        int pos=0;
        QString text=ui->lineEdit->text();
        QString medicineSql;
        dbManager DB;
        DB.openDB();
        QSqlQuery Query;
        if(v.validate(text,pos)==QValidator::Acceptable){
            qDebug()<<"匹配成功: "<<ui->lineEdit->text();
            medicineSql="select * from drug where drug_id = "+ui->lineEdit->text()+";";

        }else{
            medicineSql="select * from drug where drug_name like '%"+ui->lineEdit->text()+"%';";
        }
        Query.prepare(medicineSql);
        Query.exec();
        while(Query.next()){
            int i=0;
            ui->tableWidget->insertRow(0);
            QTableWidgetItem *drugIdItem=new QTableWidgetItem(Query.value("drug_id").toString());
            ui->tableWidget->setItem(i,0,drugIdItem);
            QTableWidgetItem *drugNameItem=new QTableWidgetItem(Query.value("drug_name").toString());
            ui->tableWidget->setItem(i,1,drugNameItem);
            QTableWidgetItem *drugPriceItem=new QTableWidgetItem(Query.value("drug_price").toString());
            ui->tableWidget->setItem(i,2,drugPriceItem);
            QTableWidgetItem *InventoryItem=new QTableWidgetItem(Query.value("virtual_inventory").toString());
            ui->tableWidget->setItem(i,3,InventoryItem);
        }
    }
    else{
        while (ui->tableWidget->rowCount()) {
            ui->tableWidget->removeRow(0);
        }
    }
}

void medicineDialog::on_tableWidget_cellDoubleClicked(int row, int column)
{
    infoDialog i(this);
    i.setModal(false);
    if(i.exec()==QDialog::Accepted){
        QString info=i.info;
        int num=i.num.toInt();
        QString drugId=ui->tableWidget->item(row,0)->text();
        QString drugName=ui->tableWidget->item(row,1)->text();
        float drugPrice=ui->tableWidget->item(row,2)->text().toFloat();
        QString Inventory=ui->tableWidget->item(row,3)->text();
        float price=num*drugPrice;
        emit setMedicineInformation(drugId,drugName,QString::number(num),price,info);
    }
}
