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
    //获取输入信息
    QString drug_id=ui->lineEdit_drugId_forPurchase->text();
    QString factory_id=ui->lineEdit_factoryId->text();
    QString drug_name=ui->lineEdit_drugName_forPurchase->text();
    QString drug_buyPrice=ui->lineEdit_drugBuyPrice->text();
    QString drug_price=ui->lineEdit_drugPrice->text();
    QString drug_number=ui->lineEdit_purchaseNumber->text();
    //判断输入信息是否完善
    if(drug_id==""||factory_id==""||drug_name==""||drug_buyPrice==""||drug_price==""||drug_number=="")
    {
        QMessageBox::warning(this,"错误","有必要信息为空");
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
    //由于同时对药品表格、操作记录表格数据进行操作，用事物保证数据的一致性。
    if(pDB->transaction()){

        //插入进货记录
        QDateTime time =QDateTime::currentDateTime();
        QString record_time=time.toString("yyyy-MM-dd hh:mm:ss");
        QString record_type="进货";
        qDebug()<<staff_id;
        QString staff_id_str=QString::number(staff_id);
        sql="insert into storecord(drug_id,drug_number,buy_price,sale_price,record_type,staff_id,record_time) values("+drug_id+","+drug_number+","+drug_buyPrice+","+drug_price+",'"+record_type+"',"+staff_id_str+",'"+record_time+"');";
        qDebug()<<sql;
        if(!query.exec(sql))
        {
            QMessageBox::warning(this,"失败","插入进货记录失败");
            if(pDB->rollback())
            {
                QMessageBox::information(this,"成功","回滚成功");
                return;
            }
            else{
                QMessageBox::warning(this,"失败","回滚失败");
                return;
            }

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
            if(!query.exec(sql))//插入失败
            {
                QMessageBox::warning(this,"失败","插入药品信息失败");
                if(pDB->rollback())
                {
                    QMessageBox::information(this,"成功","回滚成功");
                    return;
                }
                else{
                    QMessageBox::warning(this,"失败","回滚失败");
                    return;
                }

            }else{//插入成功
                QMessageBox::information(this,"成功","插入药品记录");
                if(pDB->commit())
                {
                    QMessageBox::information(this,"成功","事物提交成功，本次操作成功。");
                    return;
                }
                else{
                    QMessageBox::warning(this,"失败","事物提交失败");
                    return;
                }
            }

        }
        //获取在库药品的原进价，和售价。
        float old_buy_price=query.value("drug_buyprice").toFloat();
        float old_sale_price=query.value("drug_price").toFloat();
        while(query.next());//把NEXT指针移动完毕
        if(old_buy_price!=drug_buyPrice.toFloat()||old_sale_price!=drug_price.toFloat())
        {
            QMessageBox Msg(QMessageBox::Warning, QString::fromLocal8Bit("警告!!!"), QString::fromLocal8Bit("本次采购药品已经在库，是否，以本次进价售价为准？"));
            QAbstractButton *pYesBtn = (QAbstractButton *)Msg.addButton(QString::fromLocal8Bit("是"), QMessageBox::YesRole);
            QAbstractButton *pNoBtn = (QAbstractButton *)Msg.addButton(QString::fromLocal8Bit("否"), QMessageBox::NoRole);
            Msg.exec();
            if (Msg.clickedButton()==pYesBtn)//是,以本次的进价售价为准。
            {
                sql="update drug set real_inventory=real_inventory+"+drug_number+",virtual_inventory=virtual_inventory+"+drug_number+",drug_buyprice="+drug_buyPrice+",drug_price="+drug_price+" where drug_id="+drug_id+";";
                if(!query.exec(sql))//更新失败
                {
                    QMessageBox::warning(this,"失败","更新药品信息失败");
                    pDB->rollback();
                    return;
                }else{//更新成功
                    QMessageBox::information(this,"成功","更新药品信息成功。");
                    if(pDB->commit())
                    {
                        QMessageBox::information(this,"成功","事物提交成功，本次操作成功。");
                    }
                    else{
                        QMessageBox::warning(this,"失败","事物提交失败");
                    }
                }
                return;
            }
        }
        //药品信息已存在,update,虚库存，实库存都要增加。药品进价、售价保持不变。
        sql="update drug set real_inventory=real_inventory+"+drug_number+",virtual_inventory=virtual_inventory+"+drug_number+" where drug_id="+drug_id+";";
        if(!query.exec(sql))//更新失败
        {
            QMessageBox::warning(this,"失败","更新药品信息失败");
            pDB->rollback();
            return;
        }else{//更新成功
            QMessageBox::information(this,"成功","更新药品信息成功。");
            if(pDB->commit())
            {
                QMessageBox::information(this,"成功","事物提交成功，本次操作成功。");
            }
            else{
                QMessageBox::warning(this,"失败","事物提交失败");
            }
        }
    }

}

//-------------仓库退药----------------
void PharmacyDialog::on_pushButton_return_clicked()
{
    //获取输入信息
    QString drug_id=ui->lineEdit_drugid_forReturn->text();
    QString return_price=ui->lineEdit_returnPrice->text();
    int return_number=ui->lineEdit_returnNumber->text().toInt();
    QString return_number_str=QString::number(return_number);
    //判断输入信息是否完善
    if(return_price==""||drug_id==""||return_number==0){
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
    //获取退货药品的原库存。
    int old_inventory=query.value("virtual_inventory").toInt();//应以虚库存为准，否则影响当天缴费过，但并未取药的病人。
    if(return_number>old_inventory)
    {
        int number=return_number-old_inventory;\
        QString number_str=QString::number(number);
        QMessageBox::warning(this,"失败","退货数量不合理，超过当前库存数量,超过："+number_str);
        return;
    }
    while(query.next());//把NEXT指针移动完毕

    //在库，且退货数量合理，执行退货操作。------涉及两个药品、记录两个表格，用事物处理，保证数据一致性。
    //更新药品库存
    if(pDB->transaction())
    {
        sql="update drug set real_inventory=real_inventory-"+return_number_str+",virtual_inventory=virtual_inventory-"+return_number_str+" where drug_id="+drug_id+";";
        if(!query.exec(sql))//更新失败
        {
            QMessageBox::warning(this,"失败","更新药品库存失败");
            if(pDB->rollback())
            {
                QMessageBox::information(this,"成功","回滚成功");
                return;
                QMessageBox::warning(this,"失败","回滚失败");
            }
            else{
                return;
            }
        }
        QMessageBox::information(this,"成功","更新药品库存成功");
        //插入退货记录
        QDateTime time =QDateTime::currentDateTime();//获取当前时间
        QString record_time=time.toString("yyyy-MM-dd hh:mm:ss");
        QString record_type="退货";
        QString staff_id_str=QString::number(staff_id);
        sql="insert into storecord(drug_id,buy_price,drug_number,record_type,staff_id,record_time) values("+drug_id+","+return_price+","+return_number_str+",'"+record_type+"',"+staff_id_str+",'"+record_time+"');";
        if(!query.exec(sql))
        {
            QMessageBox::warning(this,"失败","插入退货记录失败");
            if(pDB->rollback())
            {
                QMessageBox::information(this,"成功","回滚成功");
            }
            else{
                QMessageBox::warning(this,"失败","回滚失败");
            }
        }else{
            QMessageBox::information(this,"成功","插入退货记录");
            if(pDB->commit())
            {
                QMessageBox::information(this,"成功","事物提交成功，本次操作成功。");
            }
            else{
                QMessageBox::warning(this,"失败","事物提交失败");
            }
        }
    }

}

//-----修改药品价格------
void PharmacyDialog::on_tableWidget_store_cellDoubleClicked(int row, int column)
{
    pUpdatePriceDialog = new updatePriceDialog(this);
    pUpdatePriceDialog->setModal(false);
    //声明槽函数的映射关系。
    connect(this,SIGNAL(sendDrugMessage(QString,QString,QString,QString)),pUpdatePriceDialog,SLOT(getDrugMessage(QString,QString,QString,QString)));
    //获取选中药品的信息。
    QString drug_id= ui->tableWidget_store->item(row,0)->text();
    QString drug_name=ui->tableWidget_store->item(row,1)->text();
    QString drug_buyPrice=ui->tableWidget_store->item(row,3)->text();
    QString drug_price=ui->tableWidget_store->item(row,4)->text();
    qDebug()<<"before emit "<<drug_id<<drug_name<<drug_buyPrice<<drug_price;
    emit sendDrugMessage(drug_id,drug_name,drug_buyPrice,drug_price);
    qDebug()<<"after emit ";
    //点击确定按钮，执行更新操作。
    if(pUpdatePriceDialog->exec()==QDialog::Accepted)
    {
        //获取更新对话框的值
        QString new_drug_buyPrice=pUpdatePriceDialog->MyDrugBuyPrice;
        QString new_drug_price=pUpdatePriceDialog->MyDrugSalePrice;
        qDebug()<<new_drug_buyPrice<<new_drug_price;
        //连接数据库
        dbManager db;
        if(!db.openDB())
        {
            QMessageBox::warning(this,"失败","连接数据库失败.");
            return;
        }
        //获取连接
        QSqlDatabase* pDB=db.getDB();
        //创建query
        QSqlQuery query(*pDB);
        //涉及药品表格以及操作记录表格，事物处理。
        if(pDB->transaction())
        {
            //更新药品价格
            QString sql="update drug set drug_buyprice="+new_drug_buyPrice+",drug_price="+new_drug_price+" where drug_id="+drug_id+";";
            qDebug()<<sql;
            //更新药品价格失败
            if(!query.exec(sql))
            {
                QMessageBox::warning(this,"失败","更新药品价格失败");
                if(pDB->rollback())
                {
                    QMessageBox::information(this,"成功","回滚成功");
                    return;
                }
                else
                {
                    QMessageBox::warning(this,"失败","回滚失败");
                    return;
                }
            }
            QMessageBox::information(this,"成功","更新药品价格成功");
            //插入修改记录
            QDateTime time =QDateTime::currentDateTime();//获取当前时间
            QString record_time=time.toString("yyyy-MM-dd hh:mm:ss");
            QString record_type="修改价格";
            QString staff_id_str=QString::number(staff_id);
            sql="insert into storecord(drug_id,buy_price,sale_price,record_type,staff_id,record_time) values("+drug_id+","+new_drug_buyPrice+","+new_drug_price+",'"+record_type+"',"+staff_id_str+",'"+record_time+"');";
            //插入修改记录失败
            if(!query.exec(sql))
            {
                QMessageBox::warning(this,"失败","插入修改记录失败");
                if(pDB->rollback())
                {
                    QMessageBox::information(this,"成功","回滚成功");
                }
                else
                {
                    QMessageBox::warning(this,"失败","回滚失败");
                }
            }
            else//成功，提交事物。
            {
                QMessageBox::information(this,"成功","插入修改记录成功。");
                if(pDB->commit())
                {
                    QMessageBox::information(this,"成功","事物提交成功，本次操作成功。");
                }
                else{
                    QMessageBox::warning(this,"失败","事物提交失败,本次操作无效。");
                }
            }

        }//事务处理
        else
        {
            QMessageBox::warning(this,"失败","开启事务处理失败。");
        }
    }//点击确定


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
//把实际库存的值赋给临时库存。
//未取药的缴费单，标记为status=2。
void PharmacyDialog::on_pushButton_closeDoor_clicked()
{
    QMessageBox Msg(QMessageBox::Warning, QString::fromLocal8Bit("警告!!!"), QString::fromLocal8Bit("您正在执行关闭药房的操作，将影响所有药品的库存。是否继续？"));
    QAbstractButton *pYesBtn = (QAbstractButton *)Msg.addButton(QString::fromLocal8Bit("是"), QMessageBox::YesRole);
    QAbstractButton *pNoBtn = (QAbstractButton *)Msg.addButton(QString::fromLocal8Bit("否"), QMessageBox::NoRole);
    Msg.exec();
    if (Msg.clickedButton()!=pYesBtn)
    {
        return;
    }
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
                QMessageBox::warning(this,"失败","设置缴费单状态失败,失败的缴费单号为:"+payment_id);
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

    //获取输入的缴费单号
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
    //遍历处方详情，依次将处方单上每一条药品详情的库存更新至数据库。
    //涉及药品表格，以及缴费单表格。以事物来处理本次操作。
    if(pDB->transaction()){
        while(query.next())
        {
            QSqlQuery query1(*pDB);
            QString drug_id=query.value(1).toString();
            QString drug_num=query.value(2).toString();
            QString sql1;
            if(2==status)//非当天取药，临时库存和实际库存都要减少。
            {
                sql1="update drug set real_inventory=real_inventory-"+drug_num+",virtual_inventory=virtual_inventory-"+drug_num+" where drug_id="+drug_id+";";
                if(!query1.exec(sql1))//更新库存失败，回滚。
                {
                    QMessageBox::warning(this,"发生错误","更新失败，药品号为:"+drug_id);
                    if(pDB->rollback())
                    {
                        QMessageBox::information(this,"成功","回滚成功");
                        return;
                    }
                    else
                    {
                        QMessageBox::warning(this,"失败","回滚失败");
                        return;
                    }
                }
                else//全部药品更新成功。
                {
                    QMessageBox::information(this,"成功","处方单所有药品更新库存成功。");
                }
            }//status=2
            else//status=0，当天取药，仅减少实际库存。
            {
                sql1="update drug set real_inventory=real_inventory-"+drug_num+" where drug_id="+drug_id+";";
                if(!query1.exec(sql1))//更新库存失败，回滚。
                {
                    QMessageBox::warning(this,"发生错误","更新失败，药品号为:"+drug_id);
                    if(pDB->rollback())
                    {
                        QMessageBox::information(this,"成功","回滚成功");
                        return;
                    }
                    else
                    {
                        QMessageBox::warning(this,"失败","回滚失败");
                        return;
                    }
                }
                else//全部药品更新成功.
                {
                    QMessageBox::information(this,"成功","处方单所有药品更新库存成功。");
                }
            }//status=0
        }//while
        //缴费单作废。
        sql="update payment set status=1 where payment_id='"+payment_id+"';";
        if(!query.exec(sql))
        {
            QMessageBox::warning(this,"失败","尝试作废缴费单失败");
            if(pDB->rollback())
            {
                QMessageBox::information(this,"成功","回滚成功");
                return;
            }
            else
            {
                QMessageBox::warning(this,"失败","回滚失败");
                return;
            }
        }
        QMessageBox::information(this,"成功","缴费单已作废。");
        if(pDB->commit())
        {
            QMessageBox::information(this,"成功","事物提交成功，本次操作成功。");
        }
        else{
            QMessageBox::warning(this,"失败","事物提交失败");
        }
    }
    else{//启动事物失败。
        QMessageBox::warning(this,"错误","启动处理药方详情的事物失败。");
    }


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
//-----------------删除厂商信息
void PharmacyDialog::on_tableWidget_factory_cellDoubleClicked(int row, int column)
{
    QMessageBox Msg(QMessageBox::Warning, QString::fromLocal8Bit("警告!!!"), QString::fromLocal8Bit("确定要删除该厂商的信息？"));
    QAbstractButton *pYesBtn = (QAbstractButton *)Msg.addButton(QString::fromLocal8Bit("是"), QMessageBox::YesRole);
    QAbstractButton *pNoBtn = (QAbstractButton *)Msg.addButton(QString::fromLocal8Bit("否"), QMessageBox::NoRole);
    Msg.exec();
    if (Msg.clickedButton()!=pYesBtn)//否，则不继续执行。
    {
        return;
    }
    //获取选择厂商的编号。
    QString factory_id=ui->tableWidget_factory->item(row,0)->text();
    //连接数据库
    dbManager db;
    if(!db.openDB())
    {
        QMessageBox::warning(this,"失败","连接数据库失败.");
        return;
    }
    //获取连接
    QSqlDatabase* pDB=db.getDB();
    //创建query
    QSqlQuery query(*pDB);
    //先判断是否有药品依赖于该厂商。
    QString sql="select * from drug where factory_id="+factory_id+";";
    if(!query.exec(sql))
    {
        QMessageBox::warning(this,"查询失败","查询与药品的依赖关系失败");
        return;
    }
    if(query.next())//在依赖关系
    {
        QMessageBox::warning(this,"错误","存在依赖关系，暂不允许删除该厂商信息。");
        return;
    }
    while(query.next()){}//移完指针
    sql="delete from factory where factory_id="+factory_id+";";
    if(!query.exec(sql))
    {
        QMessageBox::warning(this,"删除失败","删除厂商信息失败。");
        return;
    }
    QMessageBox::information(this,"成功","删除成功。");

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
        sql="select storecord.drug_id,drug.drug_name,storecord.buy_price,storecord.sale_price,storecord.drug_number,storecord.record_type,storecord.staff_id,storecord.record_time from drug,storecord where storecord.drug_id=drug.drug_id;";
    }
    else
    {
        sql="select storecord.drug_id,drug.drug_name,storecord.buy_price,storecord.sale_price,storecord.drug_number,storecord.record_type,storecord.staff_id,storecord.record_time from drug,storecord where storecord.drug_id=drug.drug_id and storecord.drug_id="+drug_id+";";

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
        QTableWidgetItem *p6=new QTableWidgetItem(query.value(6).toString());
        QTableWidgetItem *p7=new QTableWidgetItem(query.value(7).toString());
        ui->tableWidget_record->setItem(0,0,p0);
        ui->tableWidget_record->setItem(0,1,p1);
        ui->tableWidget_record->setItem(0,2,p2);
        ui->tableWidget_record->setItem(0,3,p3);
        ui->tableWidget_record->setItem(0,4,p4);
        ui->tableWidget_record->setItem(0,5,p5);
        ui->tableWidget_record->setItem(0,6,p6);
        ui->tableWidget_record->setItem(0,7,p7);
        while(query.next())
        {
            ui->tableWidget_record->insertRow(0);
            QTableWidgetItem *p0=new QTableWidgetItem(query.value(0).toString());
            QTableWidgetItem *p1=new QTableWidgetItem(query.value(1).toString());
            QTableWidgetItem *p2=new QTableWidgetItem(query.value(2).toString());
            QTableWidgetItem *p3=new QTableWidgetItem(query.value(3).toString());
            QTableWidgetItem *p4=new QTableWidgetItem(query.value(4).toString());
            QTableWidgetItem *p5=new QTableWidgetItem(query.value(5).toString());
            QTableWidgetItem *p6=new QTableWidgetItem(query.value(6).toString());
            QTableWidgetItem *p7=new QTableWidgetItem(query.value(7).toString());
            ui->tableWidget_record->setItem(0,0,p0);
            ui->tableWidget_record->setItem(0,1,p1);
            ui->tableWidget_record->setItem(0,2,p2);
            ui->tableWidget_record->setItem(0,3,p3);
            ui->tableWidget_record->setItem(0,4,p4);
            ui->tableWidget_record->setItem(0,5,p5);
            ui->tableWidget_record->setItem(0,6,p6);
            ui->tableWidget_record->setItem(0,7,p7);
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
        sql="select storecord.drug_id,drug.drug_name,storecord.buy_price,storecord.sale_price,storecord.drug_number,storecord.record_type,storecord.staff_id,storecord.record_time from drug,storecord where storecord.drug_id=drug.drug_id;";
    }
    else
    {

        sql="select storecord.drug_id,drug.drug_name,storecord.buy_price,storecord.sale_price,storecord.drug_number,storecord.record_type,storecord.staff_id,storecord.record_time from drug,storecord where storecord.drug_id=drug.drug_id and drug.drug_name like '%"+drug_name+"%';";

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
        QTableWidgetItem *p6=new QTableWidgetItem(query.value(6).toString());
        QTableWidgetItem *p7=new QTableWidgetItem(query.value(7).toString());
        ui->tableWidget_record->setItem(0,0,p0);
        ui->tableWidget_record->setItem(0,1,p1);
        ui->tableWidget_record->setItem(0,2,p2);
        ui->tableWidget_record->setItem(0,3,p3);
        ui->tableWidget_record->setItem(0,4,p4);
        ui->tableWidget_record->setItem(0,5,p5);
        ui->tableWidget_record->setItem(0,6,p6);
        ui->tableWidget_record->setItem(0,7,p7);
        while(query.next())
        {
            ui->tableWidget_record->insertRow(0);
            QTableWidgetItem *p0=new QTableWidgetItem(query.value(0).toString());
            QTableWidgetItem *p1=new QTableWidgetItem(query.value(1).toString());
            QTableWidgetItem *p2=new QTableWidgetItem(query.value(2).toString());
            QTableWidgetItem *p3=new QTableWidgetItem(query.value(3).toString());
            QTableWidgetItem *p4=new QTableWidgetItem(query.value(4).toString());
            QTableWidgetItem *p5=new QTableWidgetItem(query.value(5).toString());
            QTableWidgetItem *p6=new QTableWidgetItem(query.value(6).toString());
            QTableWidgetItem *p7=new QTableWidgetItem(query.value(7).toString());
            ui->tableWidget_record->setItem(0,0,p0);
            ui->tableWidget_record->setItem(0,1,p1);
            ui->tableWidget_record->setItem(0,2,p2);
            ui->tableWidget_record->setItem(0,3,p3);
            ui->tableWidget_record->setItem(0,4,p4);
            ui->tableWidget_record->setItem(0,5,p5);
            ui->tableWidget_record->setItem(0,6,p6);
            ui->tableWidget_record->setItem(0,7,p7);
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
        sql="select storecord.drug_id,drug.drug_name,storecord.buy_price,storecord.sale_price,storecord.drug_number,storecord.record_type,storecord.staff_id,storecord.record_time from drug,storecord where storecord.drug_id=drug.drug_id;";
    }
    else
    {
        sql="select storecord.drug_id,drug.drug_name,storecord.buy_price,storecord.sale_price,storecord.drug_number,storecord.record_type,storecord.staff_id,storecord.record_time from drug,storecord where storecord.drug_id=drug.drug_id and storecord.staff_id="+staff_id+";";

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
        QTableWidgetItem *p6=new QTableWidgetItem(query.value(6).toString());
        QTableWidgetItem *p7=new QTableWidgetItem(query.value(7).toString());
        ui->tableWidget_record->setItem(0,0,p0);
        ui->tableWidget_record->setItem(0,1,p1);
        ui->tableWidget_record->setItem(0,2,p2);
        ui->tableWidget_record->setItem(0,3,p3);
        ui->tableWidget_record->setItem(0,4,p4);
        ui->tableWidget_record->setItem(0,5,p5);
        ui->tableWidget_record->setItem(0,6,p6);
        ui->tableWidget_record->setItem(0,7,p7);
        while(query.next())
        {
            ui->tableWidget_record->insertRow(0);
            QTableWidgetItem *p0=new QTableWidgetItem(query.value(0).toString());
            QTableWidgetItem *p1=new QTableWidgetItem(query.value(1).toString());
            QTableWidgetItem *p2=new QTableWidgetItem(query.value(2).toString());
            QTableWidgetItem *p3=new QTableWidgetItem(query.value(3).toString());
            QTableWidgetItem *p4=new QTableWidgetItem(query.value(4).toString());
            QTableWidgetItem *p5=new QTableWidgetItem(query.value(5).toString());
            QTableWidgetItem *p6=new QTableWidgetItem(query.value(6).toString());
            QTableWidgetItem *p7=new QTableWidgetItem(query.value(7).toString());
            ui->tableWidget_record->setItem(0,0,p0);
            ui->tableWidget_record->setItem(0,1,p1);
            ui->tableWidget_record->setItem(0,2,p2);
            ui->tableWidget_record->setItem(0,3,p3);
            ui->tableWidget_record->setItem(0,4,p4);
            ui->tableWidget_record->setItem(0,5,p5);
            ui->tableWidget_record->setItem(0,6,p6);
            ui->tableWidget_record->setItem(0,7,p7);
        }
    }
    //QTableWidget根据内容调整列宽
    ui->tableWidget_record->resizeColumnsToContents();

}








