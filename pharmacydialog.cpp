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
    //清空tablewidget
    while(ui->tableWidget_store->rowCount()!=0)
    {
        ui->tableWidget_store->removeRow(0);
    }
    //组装SQL语句
    QString sql;
    if (""==ui->lineEdit_drugId_forSelect->text() &&""==ui->lineEdit_drugName_forSelect->text())//查询显示全部
    {
       QMessageBox::information(this,"OK","查询全部药品信息");
       sql="select* from drug;";

    }
    else
    {
        if (""!=ui->lineEdit_drugId_forSelect->text())//按编号查询
        {
           QMessageBox::information(this,"OK","按药品编号查询");
           QString drug_id=ui->lineEdit_drugId_forSelect->text();
           sql="select *from drug where drug_id="+drug_id+";";

        }
        else//否则，按照药品名称模糊查询
        {
            QMessageBox::information(this,"OK","按药品名称查询");
            QString drug_name=ui->lineEdit_drugName_forSelect->text();
            sql="select *from drug where drug_name like '%"+drug_name+"%';";

        }
    }
    //连接数据库
    dbManager db;
    if(!db.openDB())
    {
        QMessageBox::information(this,"失败","连接数据库失败.");
        return;
    }
    QSqlDatabase* pDB=db.getDB();//获取连接
    QSqlQuery query(*pDB);//创建query
    //执行查询
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
    //输出显示数据
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
void PharmacyDialog::getAccountAndToolTip(int account,int dpmdetail)//account为账号即员工号，dpmdetail为科室详情号
{

}
