#include "pharmacydialog.h"
#include "ui_pharmacydialog.h"
#include "dbManager.h"

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

void PharmacyDialog::on_pushButton_selectDrug_clicked()
{
    //查询显示全部
    if (""==ui->lineEdit_drugId_forSelect->text() &&""==ui->lineEdit_drugName_forSelect->text())
    {
        QString sql="select* from drug;";
        dbManager db;
        if(!db.openDB())
        {
            QMessageBox::information(this,"失败","连接数据库失败.");
            return;
        }
        QSqlDatabase* pDB=db.getDB();//获取连接
        QSqlQuery query(*pDB);//创建query
        if(!query.exec(sql))
        {
            QMessageBox::information(this,"查询失败","查询全部药品信息失败");
            return;
        }
        if(!query.next())
        {
            QMessageBox::information(this,"error","仓库药品信息记录为空");
            return;
        }
        else
        {
            ui->tableWidget_store->insertRow(0);
            QTableWidgetItem *p0=new QTableWidgetItem(query.value(0).toString());
            QTableWidgetItem *p1=new QTableWidgetItem(query.value(1).toString());
            QTableWidgetItem *p2=new QTableWidgetItem(query.value(2).toString());
            QTableWidgetItem *p3=new QTableWidgetItem(query.value(3).toString());
            QTableWidgetItem *p4=new QTableWidgetItem(query.value(4).toString());
            QTableWidgetItem *p5=new QTableWidgetItem(query.value(5).toString());
            QTableWidgetItem *p6=new QTableWidgetItem(query.value(6).toString());
            ui->tableWidget_store->setItem(0,0,p0);
            ui->tableWidget_store->setItem(0,1,p1);
            ui->tableWidget_store->setItem(0,2,p2);
            ui->tableWidget_store->setItem(0,3,p3);
            ui->tableWidget_store->setItem(0,4,p4);
            ui->tableWidget_store->setItem(0,5,p5);
            ui->tableWidget_store->setItem(0,6,p6);
            while(query.next())
            {
                ui->tableWidget_store->insertRow(0);
                QTableWidgetItem *p0=new QTableWidgetItem(query.value(0).toString());
                QTableWidgetItem *p1=new QTableWidgetItem(query.value(1).toString());
                QTableWidgetItem *p2=new QTableWidgetItem(query.value(2).toString());
                QTableWidgetItem *p3=new QTableWidgetItem(query.value(3).toString());
                QTableWidgetItem *p4=new QTableWidgetItem(query.value(4).toString());
                QTableWidgetItem *p5=new QTableWidgetItem(query.value(5).toString());
                QTableWidgetItem *p6=new QTableWidgetItem(query.value(6).toString());
                ui->tableWidget_store->setItem(0,0,p0);
                ui->tableWidget_store->setItem(0,1,p1);
                ui->tableWidget_store->setItem(0,2,p2);
                ui->tableWidget_store->setItem(0,3,p3);
                ui->tableWidget_store->setItem(0,4,p4);
                ui->tableWidget_store->setItem(0,5,p5);
                ui->tableWidget_store->setItem(0,6,p6);
            }
        }
        //QTableWidget根据内容调整列宽
        ui->tableWidget_store->resizeColumnsToContents();

    }
    //按编号查询
    if (""!=ui->lineEdit_drugId_forSelect->text())
    {
    }
    //否则，按照药品名称模糊查询

}
