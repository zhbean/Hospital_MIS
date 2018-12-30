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

//-----------------------------------------------标题栏显示-------------------------------------------------------------------------------------
void PharmacyDialog::getAccountAndToolTip(int account,int dpmdetail)//account为账号即员工号，dpmdetail为科室详情号
{
    staff_id=account;
    dbManager db;
    db.openDB();
    QStringList information=db.getStaffAndRoom(account,dpmdetail);
    if(!information.isEmpty()){
        ui->StaffIdLabel->setNum(account);
        ui->StaffNameLabel->setText(information.at(0));
        ui->RoomNameLabel->setText(information.at(1));
    }
    else{
        QMessageBox::information(NULL,"错误","数据查询错误！",QMessageBox::Yes|QMessageBox::No,QMessageBox::Yes);
    }

}




//-------------------------------------------tab药品库存管理-------------------------------------------------------


//------药品查询------
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
        QMessageBox::information(this,"查询失败","查询药品信息失败");
        return;
    }
    if(!query.next())
    {
        QMessageBox::information(this,"error","查询结果为空");
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

//-----------仓库进药---------
void PharmacyDialog::on_pushButton_purchase_clicked()
{
    //判断输入信息是否完善
    if(ui->lineEdit_drugId_forPurchase->text()==""||ui->lineEdit_drugName_forPurchase->text()==""||ui->lineEdit_factoryId->text()==""||ui->lineEdit_drugBuyPrice->text()==""||ui->lineEdit_drugPrice->text()==""||ui->lineEdit_purchaseNumber->text()=="")
    {
        QMessageBox::information(this,"错误","有必要信息为空");
        return;
    }

    //获取输入信息
    QString drug_id=ui->lineEdit_drugId_forPurchase->text();
    QString factory_id=ui->lineEdit_factoryId->text();
    QString drug_name=ui->lineEdit_drugName_forPurchase->text();
    QString drug_buyPrice=ui->lineEdit_drugBuyPrice->text();
    QString drug_price=ui->lineEdit_drugPrice->text();
    QString drug_number=ui->lineEdit_purchaseNumber->text();

    //连接数据库
    dbManager db;
    if(!db.openDB())
    {
        QMessageBox::information(this,"失败","连接数据库失败.");
        return;
    }
    QSqlDatabase* pDB=db.getDB();//获取连接
    QSqlQuery query(*pDB);//创建query

    //厂商不在库，则终止本次操作。
    QString sql;
    sql="select *from factory where factory_id="+factory_id+";";
    if(!query.exec(sql))
    {
        QMessageBox::information(this,"查询失败","查询厂商信息失败");
        return;
    }
    if(!query.next())//厂商信息不存在
    {
        QMessageBox::information(this,"失败","厂商不存在，请先添加厂商信息。");
        return;
    }
    while(query.next());//把NEXT指针移动完毕

    //插入进货记录
    QDateTime time =QDateTime::currentDateTime();
    QString record_time=time.toString("yyyy-MM-dd hh:mm:ss");
    QString record_type="进货";
    qDebug()<<staff_id;
    QString staff_id_str=QString::number(staff_id);
    sql="insert into storecord(drug_id,drug_number,record_type,staff_id,record_time) values("+drug_id+","+drug_number+",'"+record_type+"',"+staff_id_str+",'"+record_time+"');";
    qDebug()<<sql;
    if(!query.exec(sql))
    {
        QMessageBox::information(this,"失败","插入进货记录失败");
        return;
    }
    QMessageBox::information(this,"成功","插入进货记录");


    //药品信息不在库,insert
    sql="select * from drug where drug_id="+drug_id+";";
    if(!query.exec(sql))
    {
        QMessageBox::information(this,"查询失败","查询药品信息失败");
        return;
    }
    if(!query.next())//药品信息不存在，insert
    {

        sql="insert into drug values("+drug_id+",'"+drug_name+"',"+factory_id+","+drug_buyPrice+","+drug_price+","+drug_number+","+drug_number+");";
        if(!query.exec(sql))
        {
            QMessageBox::information(this,"失败","插入药品信息失败");
            return;
        }
        QMessageBox::information(this,"成功","插入药品信息");
        return;

    }
    while(query.next());//把NEXT指针移动完毕

    //药品信息已存在,update
    sql="update drug set real_inventory=real_inventory+"+drug_number+",virtual_inventory=virtual_inventory+"+drug_number+" where drug_id="+drug_id+";";
    if(!query.exec(sql))
    {
        QMessageBox::information(this,"失败","更新药品信息失败");
        return;
    }
    QMessageBox::information(this,"成功","更新药品库存");


}

//-------------仓库退药----------------



//-------------------------------------------tab操作记录管理-------------------------------------------------------

//-------------按药品编号查询记录----------------
void PharmacyDialog::on_pushButton_selectByDrugId_clicked()
{
    //清空tablewidget
    while(ui->tableWidget_record->rowCount()!=0)
    {
        ui->tableWidget_record->removeRow(0);
    }
    //组装SQL语句
    QString sql;
    QString drug_id=ui->lineEdit_recordTab_drugId->text();
    if (""==drug_id)//查询显示全部
    {
       QMessageBox::information(this,"OK","未输入药品号，查询全部记录");
       sql="select storecord.drug_id,drug.drug_name,storecord.drug_number,storecord.record_type,storecord.staff_id,storecord.record_time from drug,storecord where storecord.drug_id=drug.drug_id;";
    }
    else
    {
        sql="select storecord.drug_id,drug.drug_name,storecord.drug_number,storecord.record_type,storecord.staff_id,storecord.record_time from drug,storecord where storecord.drug_id=drug.drug_id and storecord.drug_id="+drug_id+";";

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
        QMessageBox::information(this,"查询失败","查询失败");
        return;
    }
    if(!query.next())
    {
        QMessageBox::information(this,"error","查询结果为空");
        return;
    }
    //输出显示数据
    else
    {
        QMessageBox::information(this,"OK","查询成功");
        ui->tableWidget_record->insertRow(0);
        QTableWidgetItem *p0=new QTableWidgetItem(query.value(0).toString());
        QTableWidgetItem *p1=new QTableWidgetItem(query.value(1).toString());
        QTableWidgetItem *p2=new QTableWidgetItem(query.value(2).toString());
        QTableWidgetItem *p3=new QTableWidgetItem(query.value(3).toString());
        QTableWidgetItem *p4=new QTableWidgetItem(query.value(4).toString());
        QTableWidgetItem *p5=new QTableWidgetItem(query.value(5).toString());
        ui->tableWidget_record->setItem(0,0,p0);
        ui->tableWidget_record->setItem(0,1,p1);
        ui->tableWidget_record->setItem(0,2,p2);
        ui->tableWidget_record->setItem(0,3,p3);
        ui->tableWidget_record->setItem(0,4,p4);
        ui->tableWidget_record->setItem(0,5,p5);
        while(query.next())
        {
            ui->tableWidget_record->insertRow(0);
            QTableWidgetItem *p0=new QTableWidgetItem(query.value(0).toString());
            QTableWidgetItem *p1=new QTableWidgetItem(query.value(1).toString());
            QTableWidgetItem *p2=new QTableWidgetItem(query.value(2).toString());
            QTableWidgetItem *p3=new QTableWidgetItem(query.value(3).toString());
            QTableWidgetItem *p4=new QTableWidgetItem(query.value(4).toString());
            QTableWidgetItem *p5=new QTableWidgetItem(query.value(5).toString());
            ui->tableWidget_record->setItem(0,0,p0);
            ui->tableWidget_record->setItem(0,1,p1);
            ui->tableWidget_record->setItem(0,2,p2);
            ui->tableWidget_record->setItem(0,3,p3);
            ui->tableWidget_record->setItem(0,4,p4);
            ui->tableWidget_record->setItem(0,5,p5);
        }
    }
    //QTableWidget根据内容调整列宽
    ui->tableWidget_record->resizeColumnsToContents();
}
//-------------按药品名称查询记录----------------
void PharmacyDialog::on_pushButton_recordTab_selectByDrugName_clicked()
{
    //清空tablewidget
    while(ui->tableWidget_record->rowCount()!=0)
    {
        ui->tableWidget_record->removeRow(0);
    }
    //组装SQL语句
    QString sql;
    QString drug_name=ui->lineEdit_recordTab_drugName->text();
    if (""==drug_name)//查询显示全部
    {
       QMessageBox::information(this,"OK","未输入药品名称，查询全部记录");
       sql="select storecord.drug_id,drug.drug_name,storecord.drug_number,storecord.record_type,storecord.staff_id,storecord.record_time from drug,storecord where storecord.drug_id=drug.drug_id;";
    }
    else
    {

        sql="select storecord.drug_id,drug.drug_name,storecord.drug_number,storecord.record_type,storecord.staff_id,storecord.record_time from drug,storecord where storecord.drug_id=drug.drug_id and drug.drug_name like '%"+drug_name+"%';";

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
        QMessageBox::information(this,"查询失败","查询失败");
        return;
    }
    if(!query.next())
    {
        QMessageBox::information(this,"error","查询结果为空");
        return;
    }
    //输出显示数据
    else
    {
        QMessageBox::information(this,"OK","查询成功");
        ui->tableWidget_record->insertRow(0);
        QTableWidgetItem *p0=new QTableWidgetItem(query.value(0).toString());
        QTableWidgetItem *p1=new QTableWidgetItem(query.value(1).toString());
        QTableWidgetItem *p2=new QTableWidgetItem(query.value(2).toString());
        QTableWidgetItem *p3=new QTableWidgetItem(query.value(3).toString());
        QTableWidgetItem *p4=new QTableWidgetItem(query.value(4).toString());
        QTableWidgetItem *p5=new QTableWidgetItem(query.value(5).toString());
        ui->tableWidget_record->setItem(0,0,p0);
        ui->tableWidget_record->setItem(0,1,p1);
        ui->tableWidget_record->setItem(0,2,p2);
        ui->tableWidget_record->setItem(0,3,p3);
        ui->tableWidget_record->setItem(0,4,p4);
        ui->tableWidget_record->setItem(0,5,p5);
        while(query.next())
        {
            ui->tableWidget_record->insertRow(0);
            QTableWidgetItem *p0=new QTableWidgetItem(query.value(0).toString());
            QTableWidgetItem *p1=new QTableWidgetItem(query.value(1).toString());
            QTableWidgetItem *p2=new QTableWidgetItem(query.value(2).toString());
            QTableWidgetItem *p3=new QTableWidgetItem(query.value(3).toString());
            QTableWidgetItem *p4=new QTableWidgetItem(query.value(4).toString());
            QTableWidgetItem *p5=new QTableWidgetItem(query.value(5).toString());
            ui->tableWidget_record->setItem(0,0,p0);
            ui->tableWidget_record->setItem(0,1,p1);
            ui->tableWidget_record->setItem(0,2,p2);
            ui->tableWidget_record->setItem(0,3,p3);
            ui->tableWidget_record->setItem(0,4,p4);
            ui->tableWidget_record->setItem(0,5,p5);
        }
    }
    //QTableWidget根据内容调整列宽
    ui->tableWidget_record->resizeColumnsToContents();

}





//-------------按员工编号查询记录----------------
void PharmacyDialog::on_pushButton_selectByStaffId_clicked()
{
    //清空tablewidget
    while(ui->tableWidget_record->rowCount()!=0)
    {
        ui->tableWidget_record->removeRow(0);
    }
    //组装SQL语句
    QString sql;
    QString staff_id=ui->lineEdit_recordTab_staffId->text();
    if (""==staff_id)//查询显示全部
    {
       QMessageBox::information(this,"OK","未输入员工号，查询全部记录");
       sql="select storecord.drug_id,drug.drug_name,storecord.drug_number,storecord.record_type,storecord.staff_id,storecord.record_time from drug,storecord where storecord.drug_id=drug.drug_id;";
    }
    else
    {
        sql="select storecord.drug_id,drug.drug_name,storecord.drug_number,storecord.record_type,storecord.staff_id,storecord.record_time from drug,storecord where storecord.drug_id=drug.drug_id and storecord.staff_id="+staff_id+";";

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
        QMessageBox::information(this,"查询失败","查询失败");
        return;
    }
    if(!query.next())
    {
        QMessageBox::information(this,"error","查询结果为空");
        return;
    }
    //输出显示数据
    else
    {
        QMessageBox::information(this,"OK","查询成功");
        ui->tableWidget_record->insertRow(0);
        QTableWidgetItem *p0=new QTableWidgetItem(query.value(0).toString());
        QTableWidgetItem *p1=new QTableWidgetItem(query.value(1).toString());
        QTableWidgetItem *p2=new QTableWidgetItem(query.value(2).toString());
        QTableWidgetItem *p3=new QTableWidgetItem(query.value(3).toString());
        QTableWidgetItem *p4=new QTableWidgetItem(query.value(4).toString());
        QTableWidgetItem *p5=new QTableWidgetItem(query.value(5).toString());
        ui->tableWidget_record->setItem(0,0,p0);
        ui->tableWidget_record->setItem(0,1,p1);
        ui->tableWidget_record->setItem(0,2,p2);
        ui->tableWidget_record->setItem(0,3,p3);
        ui->tableWidget_record->setItem(0,4,p4);
        ui->tableWidget_record->setItem(0,5,p5);
        while(query.next())
        {
            ui->tableWidget_record->insertRow(0);
            QTableWidgetItem *p0=new QTableWidgetItem(query.value(0).toString());
            QTableWidgetItem *p1=new QTableWidgetItem(query.value(1).toString());
            QTableWidgetItem *p2=new QTableWidgetItem(query.value(2).toString());
            QTableWidgetItem *p3=new QTableWidgetItem(query.value(3).toString());
            QTableWidgetItem *p4=new QTableWidgetItem(query.value(4).toString());
            QTableWidgetItem *p5=new QTableWidgetItem(query.value(5).toString());
            ui->tableWidget_record->setItem(0,0,p0);
            ui->tableWidget_record->setItem(0,1,p1);
            ui->tableWidget_record->setItem(0,2,p2);
            ui->tableWidget_record->setItem(0,3,p3);
            ui->tableWidget_record->setItem(0,4,p4);
            ui->tableWidget_record->setItem(0,5,p5);
        }
    }
    //QTableWidget根据内容调整列宽
    ui->tableWidget_record->resizeColumnsToContents();

}


