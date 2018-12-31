#include "historymedicinedialog.h"
#include "ui_historymedicinedialog.h"

historyMedicineDialog::historyMedicineDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::historyMedicineDialog)
{
    ui->setupUi(this);
    ui->tableWidget->setColumnWidth(0,100);
    ui->tableWidget->setColumnWidth(1,250);
    ui->tableWidget->setColumnWidth(2,80);
    ui->tableWidget->setColumnWidth(3,120);
    ui->tableWidget->setColumnWidth(4,250);
    ui->tableWidget->setColumnWidth(5,50);
    ui->tableWidget->setColumnWidth(6,50);

    ui->tableWidget->verticalHeader()->hide();
}

historyMedicineDialog::~historyMedicineDialog()
{
    delete ui;
}

void historyMedicineDialog::on_buttonBox_accepted()
{
    for(int i=0;i<ui->tableWidget->rowCount();i++){
        if(ui->tableWidget->item(i,6)->checkState()){
            this->medicineId.insert(0,ui->tableWidget->item(i,0)->text());
            this->medicineName.insert(0,ui->tableWidget->item(i,1)->text());
            this->medicinenum.insert(0,ui->tableWidget->item(i,2)->text());
            this->medicinePrice.insert(0,ui->tableWidget->item(i,3)->text());
            this->medicineinfo.insert(0,ui->tableWidget->item(i,4)->text());

        }
    }
}
void historyMedicineDialog::getPspId(QString pspId){
    QString sql="select * from pspdetail,drug where pspdetail.drug_id=drug.drug_id and psp_id='"+pspId+"';";
    dbManager db;
    db.openDB();
    QSqlQuery Query;
    Query.prepare(sql);
    Query.exec();
    while (Query.next()) {
        QString drugId=Query.value("drug_id").toString();
        QString drugName=Query.value("drug_name").toString();
        QString drugNum=Query.value("drug_num").toString();
        QString pspPrice=Query.value("psp_price").toString();
        QString inventory=Query.value("virtual_inventory").toString();
        QString pspInfo=Query.value("psp_info").toString();
        QTableWidgetItem *drugIdItem=new QTableWidgetItem(drugId);
        QTableWidgetItem *drugNameItem=new QTableWidgetItem(drugName);
        QTableWidgetItem *drugNumItem=new QTableWidgetItem(drugNum);
        QTableWidgetItem *pspPriceItem=new QTableWidgetItem(pspPrice);
        QTableWidgetItem *pspInfoItem=new QTableWidgetItem(pspInfo);
        QTableWidgetItem *inventoryItem=new QTableWidgetItem(inventory);
        QTableWidgetItem *selectItem=new QTableWidgetItem();
        QCheckBox check;
        if(inventory>0){
            selectItem->setCheckState(check.checkState());
        }
        ui->tableWidget->insertRow(0);
        ui->tableWidget->setItem(0,0,drugIdItem);
        ui->tableWidget->setItem(0,1,drugNameItem);
        ui->tableWidget->setItem(0,2,drugNumItem);
        ui->tableWidget->setItem(0,3,pspPriceItem);
        ui->tableWidget->setItem(0,4,pspInfoItem);
        ui->tableWidget->setItem(0,5,inventoryItem);
        ui->tableWidget->setItem(0,6,selectItem);
    }
}
