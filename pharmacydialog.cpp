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

//-----------仓库进药---------
void PharmacyDialog::on_pushButton_purchase_clicked()
{
    //判断输入信息是否完善
    if(ui->lineEdit_drugId_forPurchase->text()==""||ui->lineEdit_drugName_forPurchase->text()==""||ui->lineEdit_factoryId->text()==""||ui->lineEdit_drugBuyPrice->text()==""||ui->lineEdit_drugPrice->text()==""||ui->lineEdit_purchaseNumber->text()=="")
    {
        QMessageBox::warning(this,"错误","有必要信息为空");
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
        QMessageBox::warning(this,"失败","连接数据库失败.");
        return;
    }
    QSqlDatabase* pDB=db.getDB();//获取连接
    QSqlQuery query(*pDB);//创建query

    //厂商不在库，则终止本次操作。
    QString sql;
    sql="select *from factory where factory_id="+factory_id+";";
    if(!query.exec(sql))
    {
        QMessageBox::warning(this,"查询失败","查询厂商信息失败");
        return;
    }
    if(!query.next())//厂商信息不存在
    {
        QMessageBox::warning(this,"失败","厂商不存在，请先添加厂商信息。");
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
        QMessageBox::warning(this,"失败","插入进货记录失败");
        return;
    }
    QMessageBox::information(this,"成功","插入进货记录");


    //药品信息不在库,insert
    sql="select * from drug where drug_id="+drug_id+";";
    if(!query.exec(sql))
    {
        QMessageBox::warning(this,"查询失败","查询药品信息失败");
        return;
    }
    if(!query.next())//药品信息不存在，insert
    {

        sql="insert into drug values("+drug_id+",'"+drug_name+"',"+factory_id+","+drug_buyPrice+","+drug_price+","+drug_number+","+drug_number+");";
        if(!query.exec(sql))
        {
            QMessageBox::warning(this,"失败","插入药品信息失败");
            return;
        }
        QMessageBox::information(this,"成功","插入药品信息");
        return;

    }
    while(query.next());//把NEXT指针移动完毕

    //药品信息已存在,update,虚库存，实库存都要增加。
    sql="update drug set real_inventory=real_inventory+"+drug_number+",virtual_inventory=virtual_inventory+"+drug_number+" where drug_id="+drug_id+";";
    if(!query.exec(sql))
    {
        QMessageBox::warning(this,"失败","更新药品信息失败");
        return;
    }
    QMessageBox::information(this,"成功","更新药品库存");


}

//-------------仓库退药----------------
void PharmacyDialog::on_pushButton_return_clicked()
{
    //获取输入信息
    QString drug_id=ui->lineEdit_drugid_forReturn->text();
    int return_number=ui->lineEdit_returnNumber->text().toInt();
    QString return_number_str=QString::number(return_number);
    //判断输入信息是否完善
    if(drug_id==""||return_number==0){
        QMessageBox::warning(this,"错误","请您填写完整的退货信息。");
        return;
    }
    //先判断药品是否在库
    QString sql="select *from drug where drug_id="+drug_id+";";
    //连接数据库
    dbManager db;
    if(!db.openDB())
    {
        QMessageBox::warning(this,"失败","连接数据库失败.");
        return;
    }
    QSqlDatabase* pDB=db.getDB();//获取连接
    QSqlQuery query(*pDB);//创建query
    //药品不在库，则终止本次操作。
    if(!query.exec(sql))
    {
        QMessageBox::warning(this,"查询失败","查询药品信息失败");
        return;
    }
    if(!query.next())//药品信息不存在
    {
        QMessageBox::warning(this,"失败","输入的药品编号错误，其数据不在库。");
        return;
    }
    //药品在库，判断退货数量是否合理。
    int old_inventory=query.value(6).toInt();//应以虚库存为准，否则影响缴费过，但并未取药的病人。
    if(return_number>old_inventory)
    {
         int number=return_number-old_inventory;
         QMessageBox::warning(this,"失败","退货数量不合理，超过当前库存数量,超过："+number);
         return;
    }
    while(query.next());//把NEXT指针移动完毕

    //在库，且退货数量合理，执行退货操作。
    //更新药品库存
    sql="update drug set real_inventory=real_inventory-"+return_number_str+",virtual_inventory=virtual_inventory-"+return_number_str+" where drug_id="+drug_id+";";
    if(!query.exec(sql))
    {
        QMessageBox::warning(this,"失败","更新药品库存失败");
        return;
    }
    QMessageBox::information(this,"成功","更新药品库存");
    //插入退货记录
    QDateTime time =QDateTime::currentDateTime();//获取当前时间
    QString record_time=time.toString("yyyy-MM-dd hh:mm:ss");
    QString record_type="退货";
    QString staff_id_str=QString::number(staff_id);
    sql="insert into storecord(drug_id,drug_number,record_type,staff_id,record_time) values("+drug_id+","+return_number_str+",'"+record_type+"',"+staff_id_str+",'"+record_time+"');";
    qDebug()<<sql;
    if(!query.exec(sql))
    {
        QMessageBox::warning(this,"失败","插入退货记录失败");
        return;
    }
    QMessageBox::information(this,"成功","插入退货记录");

}

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
        QMessageBox::warning(this,"失败","连接数据库失败.");
        return;
    }
    QSqlDatabase* pDB=db.getDB();//获取连接
    QSqlQuery query(*pDB);//创建query
    //执行查询
    if(!query.exec(sql))
    {
        QMessageBox::warning(this,"查询失败","查询药品信息失败");
        return;
    }
    if(!query.next())
    {
        QMessageBox::warning(this,"error","查询结果为空");
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

//-------------药房关闭，更新药品库存，保证数据一致性。
//即把实际库存的值赋给临时库存。且，未取药的缴费单，标记为status=2。
void PharmacyDialog::on_pushButton_closeDoor_clicked()
{
    //更新库存
    QString sql="update drug set  virtual_inventory=real_inventory;";
    //连接数据库
    dbManager db;
    if(!db.openDB())
    {
        QMessageBox::warning(this,"失败","连接数据库失败.");
        return;
    }
    QSqlDatabase* pDB=db.getDB();//获取连接
    QSqlQuery query(*pDB);//创建query
    //更新库存失败
    if(!query.exec(sql))
    {
        QMessageBox::warning(this,"失败","更新仓库药品库存信息失败");
        return;
    }
    //设置缴费单状态
    sql="select * from payment;";
    if(!query.exec(sql))
    {
        QMessageBox::warning(this,"错误","查询缴费单记录失败");
        return;
    }
    if(!query.next())//药品信息不存在
    {
        QMessageBox::warning(this,"失败","缴费单表s格为空。");
        return;
    }
    //获第一条缴费单记录的状态和编号
    QString payment_id1=query.value("payment_id").toString();
    int status1=query.value("status").toInt();
    //判断缴费单状态
    QSqlQuery query1(*pDB);
    QString sql1="update payment set status=2 where payment_id='"+payment_id1+"';";
    if(status1==0)//药房关闭，还未取药。
    {
        if(!query1.exec(sql1))
        {
             QMessageBox::warning(this,"失败","设置第一条缴费单状态失败");
             return;
        }
    }
    //遍历所有缴费单记录
    while(query.next())
    {
        QString payment_id=query.value("payment_id").toString();
        int status=query.value("status").toInt();
        QString sql2="update payment set status=2 where payment_id='"+payment_id+"';";
        if(status==0)//药房关闭，还未取药。
        {
            if(!query1.exec(sql2))
            {
                 QMessageBox::warning(this,"失败","设置第一条缴费单状态失败");
                 return;
            }
        }
    }
     QMessageBox::information(this,"成功","药品库存更新完毕，可以正常关闭药房。");


}

//-------------------------------------------tab药品发放管理-------------------------------------------------------

//查询病人缴费单
void PharmacyDialog::on_pushButton_tabSale_selectPayment_clicked()
{
    //清空tablewidget
    while(ui->tableWidget_sale->rowCount()!=0)
    {
        ui->tableWidget_sale->removeRow(0);
    }
    //获取录入的病人身份证号
    QString ID=ui->lineEdit_tabSale_patientId->text();
    if(""==ID)
    {
        QMessageBox::warning(this,"错误","请先录入病人身份证号码。");
        return;
    }
    //根据病人身份证号，连接处方单表和缴费单表。
    QString sql="select payment.payment_id,payment.psp_id,payment.staff_id,payment.sum_price,payment.payment_date,payment.status from payment,psp where payment.psp_id=psp.psp_id and psp.ID='"+ID+"';";
    //连接数据库
    dbManager db;
    if(!db.openDB())
    {
        QMessageBox::warning(this,"失败","连接数据库失败.");
        return;
    }
    QSqlDatabase* pDB=db.getDB();//获取连接
    QSqlQuery query(*pDB);//创建query
    //执行查询
    qDebug()<<"sql:"<<sql;
    if(!query.exec(sql))
    {
        QMessageBox::warning(this,"查询失败","查询失败");
        return;
    }
    if(!query.next())
    {
        QMessageBox::warning(this,"error","查询结果为空");
        return;
    }
    //输出显示数据
    else
    {
        //填充tablewidget
        ui->tableWidget_sale->insertRow(0);
        QTableWidgetItem *p0=new QTableWidgetItem(query.value(0).toString());
        QTableWidgetItem *p1=new QTableWidgetItem(query.value(1).toString());
        QTableWidgetItem *p2=new QTableWidgetItem(query.value(2).toString());
        QTableWidgetItem *p3=new QTableWidgetItem(query.value(3).toString());
        QTableWidgetItem *p4=new QTableWidgetItem(query.value(4).toString());
        QTableWidgetItem *p5=new QTableWidgetItem(query.value(5).toString());
        ui->tableWidget_sale->setItem(0,0,p0);
        ui->tableWidget_sale->setItem(0,1,p1);
        ui->tableWidget_sale->setItem(0,2,p2);
        ui->tableWidget_sale->setItem(0,3,p3);
        ui->tableWidget_sale->setItem(0,4,p4);
        ui->tableWidget_sale->setItem(0,5,p5);
        while(query.next())
        {
            ui->tableWidget_sale->insertRow(0);
            QTableWidgetItem *p0=new QTableWidgetItem(query.value(0).toString());
            QTableWidgetItem *p1=new QTableWidgetItem(query.value(1).toString());
            QTableWidgetItem *p2=new QTableWidgetItem(query.value(2).toString());
            QTableWidgetItem *p3=new QTableWidgetItem(query.value(3).toString());
            QTableWidgetItem *p4=new QTableWidgetItem(query.value(4).toString());
            QTableWidgetItem *p5=new QTableWidgetItem(query.value(5).toString());
            ui->tableWidget_sale->setItem(0,0,p0);
            ui->tableWidget_sale->setItem(0,1,p1);
            ui->tableWidget_sale->setItem(0,2,p2);
            ui->tableWidget_sale->setItem(0,3,p3);
            ui->tableWidget_sale->setItem(0,4,p4);
            ui->tableWidget_sale->setItem(0,5,p5);

        }
    }
    //QTableWidget根据内容调整列宽
    ui->tableWidget_sale->resizeColumnsToContents();

}

//发放药品
void PharmacyDialog::on_pushButton_tabSale_grant_clicked()
{

    //获取输入的缴费单号（模拟刷病人卡获得）
    QString payment_id=ui->lineEdit_tabSale_paymentId->text();
    if(payment_id=="")
    {
        QMessageBox::warning(this,"错误","请您先输入缴费单号。");
        return;
    }
    //连接数据库
    dbManager db;
    if(!db.openDB())
    {
        QMessageBox::warning(this,"失败","连接数据库失败.");
        return;
    }
    QSqlDatabase* pDB=db.getDB();//获取连接
    QSqlQuery query(*pDB);//创建query
    //根据用户输入的缴费单号，获取处方单号，以及缴费单状态。
    QString sql="select *from payment where payment_id='"+payment_id+"';";
    qDebug()<<"sql:"<<sql;
    if(!query.exec(sql))
    {
        QMessageBox::warning(this,"查询失败","查询处方单号失败");
        return;
    }
    if(!query.next())
    {
        QMessageBox::warning(this,"error","查询结果为空,请输入正确的缴费单号。");
        return;
    }
    //拿到药方单号
    QString psp_id=query.value(1).toString();
    //获取缴费单状态
    int status=query.value("status").toInt();
    while(query.next());//把指针移动完毕，防止数据库被锁。
    if(1==status)//已经取过药
    {
        QMessageBox::warning(this,"失败","缴费单已失效");
        return;
    }
    //根据处方单号，查询处方详情
    sql="select * from pspdetail where psp_id='"+psp_id+"';";
    if(!query.exec(sql))
    {
        QMessageBox::warning(this,"查询失败","处方详情失败");
        return;
    }
    while(query.next())
    {
         QSqlQuery query1(*pDB);
         QString drug_id=query.value(1).toString();
         QString drug_num=query.value(2).toString();
         QString sql1;
         if(2==status)//隔天未取药，临时库存和实际库存都要减少。
         {
             sql1="update drug set real_inventory=real_inventory-"+drug_num+",virtual_inventory=virtual_inventory-"+drug_num+" where drug_id="+drug_id+";";
             if(!query1.exec(sql1))
             {
                 QMessageBox::warning(this,"发生错误","库存不足，药品号为:"+drug_id);
                 return;
             }
         }

         else
         {
             sql1="update drug set real_inventory=real_inventory-"+drug_num+" where drug_id="+drug_id+";";
             if(!query1.exec(sql1))
             {
                 QMessageBox::warning(this,"发生错误","更新失败，药品号为:"+drug_id);
                 return;
             }
         }


    }
    //缴费单作废。
    sql="update payment set status=1 where payment_id='"+payment_id+"';";
    if(!query.exec(sql))
    {
        QMessageBox::warning(this,"失败","尝试作废缴费单失败");
        return;
    }
    QMessageBox::information(this,"成功","药品库存更新成功，缴费单已作废。");


}

//查询缴费单详情
void PharmacyDialog::on_tableWidget_sale_cellDoubleClicked(int row, int column)
{
    //清空List，并添加标题
    ui->listWidget_tabSale->clear();
    ui->listWidget_tabSale->addItem("                           缴费单药品详情");
    ui->listWidget_tabSale->addItem("编号    名称    数量    用法");
    //获取选择的药方单号
    QString psp_id=ui->tableWidget_sale->item(row,1)->text();
    //根据药方单号，查询药方详情。
    QString sql="select  pspdetail.drug_id,drug.drug_name,pspdetail.drug_num,pspdetail.psp_info from pspdetail,drug where drug.drug_id=pspdetail.drug_id and pspdetail.psp_id='"+psp_id+"';";
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
    qDebug()<<"sql:"<<sql;
    if(!query.exec(sql))
    {
        QMessageBox::information(this,"失败","查询缴费单详情失败");
        return;
    }
    if(!query.next())
    {
        QMessageBox::information(this,"错误","缴费单详情为空");
        return;
    }
    QString drug_id=query.value(0).toString();
    QString drug_name=query.value(1).toString();
    QString drug_number=query.value(2).toString();
    QString psp_info=query.value(3).toString();
    QString str=drug_id+"    "+drug_name+"    "+drug_number+"  "+psp_info;
    ui->listWidget_tabSale->addItem(str);
    while(query.next())
    {
        QString drug_id=query.value(0).toString();
        QString drug_name=query.value(1).toString();
        QString drug_number=query.value(2).toString();
        QString psp_info=query.value(3).toString();
        QString str=drug_id+"    "+drug_name+"    "+drug_number+"  "+psp_info;
        ui->listWidget_tabSale->addItem(str);
    }

}




//-------------------------------------------tab出厂商管理-------------------------------------------------------

//------------添加出厂商----------------
void PharmacyDialog::on_pushButton_addFactory_clicked()
{
    //获取输入信息
    QString factory_name=ui->lineEdit_tabFactory_factoryName_forAdd->text();
    QString factory_address=ui->lineEdit_tabFactory_factoryAddress_forAdd->text();
    QString factory_connect=ui->lineEdit_tabFactory_factoryConn_forAdd->text();
    //判断输入信息是否完善
    if(factory_name==""||factory_address==""||factory_connect=="")
    {
        QMessageBox::warning(this,"错误","有必要信息为空");
        return;
    }
    QString sql="insert into factory(factory_name,factory_address,factory_contact) values('"+factory_name+"','"+factory_address+"','"+factory_connect+"');";
    //连接数据库
    dbManager db;
    if(!db.openDB())
    {
        QMessageBox::warning(this,"失败","连接数据库失败.");
        return;
    }
    QSqlDatabase* pDB=db.getDB();//获取连接
    QSqlQuery query(*pDB);//创建query
    //执行查询
    if(!query.exec(sql))
    {
        QMessageBox::warning(this,"失败","插入出厂商信息失败");
        return;
    }
    QMessageBox::information(this,"成功","添加成功");

}


//-----------更新厂商信息----------------
void PharmacyDialog::on_pushButton_tabFactory_UpdateFactory_clicked()
{
    //获取输入信息
    QString factory_id=ui->lineEdit_tabFactory_factoryId_forUpdate->text();
    QString factory_name=ui->lineEdit_tabFactory_factoryName_forUpdate->text();
    QString factory_address=ui->lineEdit_tabFactory_factoryAddress_forUpdate->text();
    QString factory_contact=ui->lineEdit_tabFactory_factoryConn_forUpdate->text();
    //判断输入信息是否完善
    if(factory_id==""||factory_name==""||factory_address==""||factory_contact=="")
    {
        QMessageBox::warning(this,"错误","有必要信息为空");
        return;
    }
    //指定厂商号不存在，则终止本次操作。
    QString sql;
    sql="select *from factory where factory_id="+factory_id+";";
    dbManager db; //连接数据库
    if(!db.openDB())
    {
        QMessageBox::warning(this,"失败","连接数据库失败.");
        return;
    }
    QSqlDatabase* pDB=db.getDB();//获取连接
    QSqlQuery query(*pDB);//创建query
    if(!query.exec(sql))
    {
        QMessageBox::warning(this,"查询失败","查询厂商信息失败");
        return;
    }
    if(!query.next())//厂商信息不存在
    {
        QMessageBox::warning(this,"失败","厂商不存在，请输入正确的厂商号。");
        return;
    }
    while(query.next());//把NEXT指针移动完毕

    //更新厂商信息
    sql="update factory set factory_name='"+factory_name+"',factory_address='"+factory_address+"',factory_contact='"+factory_contact+"' where factory_id="+factory_id+";";
    qDebug()<<"sql:"<<sql;
    if(!query.exec(sql))
    {
        QMessageBox::warning(this,"失败","更新厂商信息失败");
        return;
    }
    QMessageBox::information(this,"成功","更新成功");


}

//-----------查询厂商信息----------------
void PharmacyDialog::on_pushButton_tabFactory_select_clicked()
{
    //清空tablewidget
    while(ui->tableWidget_factory->rowCount()!=0)
    {
        ui->tableWidget_factory->removeRow(0);
    }
    //获取输入信息
    QString factory_id=ui->lineEdit_tabFactory_factoryID_forSelect->text();
    QString factory_name=ui->lineEdit_tabFactory_factoryName_forSelect->text();
    //组装SQL语句
    QString sql;
    if (""==factory_id &&""==factory_name)//查询显示全部
    {
       QMessageBox::information(this,"OK","查询全部出厂商信息");
       sql="select* from factory;";

    }
    else
    {
        if (""!=factory_id)//按编号查询
        {
           QMessageBox::information(this,"OK","按药品编号查询");
           sql="select *from factory where factory_id="+factory_id+";";

        }
        else//否则，按照药品名称模糊查询
        {
            QMessageBox::information(this,"OK","按药品名称查询");
            sql="select *from factory where factory_name like '%"+factory_name+"%';";

        }
    }
    //连接数据库
    dbManager db;
    if(!db.openDB())
    {
        QMessageBox::warning(this,"失败","连接数据库失败.");
        return;
    }
    QSqlDatabase* pDB=db.getDB();//获取连接
    QSqlQuery query(*pDB);//创建query
    //执行查询
    qDebug()<<"sql:"<<sql;
    if(!query.exec(sql))
    {
        QMessageBox::warning(this,"查询失败","查询失败");
        return;
    }
    if(!query.next())
    {
        QMessageBox::warning(this,"error","查询结果为空");
        return;
    }
    //输出显示数据
    else
    {
        ui->tableWidget_factory->insertRow(0);
        QTableWidgetItem *p0=new QTableWidgetItem(query.value(0).toString());
        QTableWidgetItem *p1=new QTableWidgetItem(query.value(1).toString());
        QTableWidgetItem *p2=new QTableWidgetItem(query.value(2).toString());
        QTableWidgetItem *p3=new QTableWidgetItem(query.value(3).toString());
        ui->tableWidget_factory->setItem(0,0,p0);
        ui->tableWidget_factory->setItem(0,1,p1);
        ui->tableWidget_factory->setItem(0,2,p2);
        ui->tableWidget_factory->setItem(0,3,p3);
        while(query.next())
        {
            ui->tableWidget_factory->insertRow(0);
            QTableWidgetItem *p0=new QTableWidgetItem(query.value(0).toString());
            QTableWidgetItem *p1=new QTableWidgetItem(query.value(1).toString());
            QTableWidgetItem *p2=new QTableWidgetItem(query.value(2).toString());
            QTableWidgetItem *p3=new QTableWidgetItem(query.value(3).toString());
            ui->tableWidget_factory->setItem(0,0,p0);
            ui->tableWidget_factory->setItem(0,1,p1);
            ui->tableWidget_factory->setItem(0,2,p2);
            ui->tableWidget_factory->setItem(0,3,p3);

        }
    }
    //QTableWidget根据内容调整列宽
    ui->tableWidget_factory->resizeColumnsToContents();
}



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
        QMessageBox::warning(this,"失败","连接数据库失败.");
        return;
    }
    QSqlDatabase* pDB=db.getDB();//获取连接
    QSqlQuery query(*pDB);//创建query
    //执行查询
    if(!query.exec(sql))
    {
        QMessageBox::warning(this,"查询失败","查询失败");
        return;
    }
    if(!query.next())
    {
        QMessageBox::warning(this,"error","查询结果为空");
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
        QMessageBox::warning(this,"失败","连接数据库失败.");
        return;
    }
    QSqlDatabase* pDB=db.getDB();//获取连接
    QSqlQuery query(*pDB);//创建query
    //执行查询
    if(!query.exec(sql))
    {
        QMessageBox::warning(this,"查询失败","查询失败");
        return;
    }
    if(!query.next())
    {
        QMessageBox::warning(this,"error","查询结果为空");
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
        QMessageBox::warning(this,"失败","连接数据库失败.");
        return;
    }
    QSqlDatabase* pDB=db.getDB();//获取连接
    QSqlQuery query(*pDB);//创建query
    //执行查询
    if(!query.exec(sql))
    {
        QMessageBox::warning(this,"查询失败","查询失败");
        return;
    }
    if(!query.next())
    {
        QMessageBox::warning(this,"error","查询结果为空");
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




