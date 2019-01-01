#include "diagnosiswindow.h"
#include "patientdialog.h"
#include <QRegExp>
#include <QRegExpValidator>
#include <QTableWidget>
#include <QHeaderView>
#include "historymedicinedialog.h"

diagnosisWindow::diagnosisWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::diagnosisWindow)
{
    ui->setupUi(this);
    ui->menubar->addAction("病人刷卡",this,SLOT(showStatistics()));
    ui->AddButton->setEnabled(false);
    ui->DelButton->setEnabled(false);
    ui->OKButton->setEnabled(false);
    ui->PrintButton->setEnabled(false);
}

diagnosisWindow::~diagnosisWindow()
{
    delete ui;
}
void diagnosisWindow::getAccountAndToolTip(int account,int dpmdetail) //account为账号即员工号，dpmdetail为科室详情号
{
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
void diagnosisWindow::showStatistics(){
    patientDialog p(this);
    p.setModal(true);
    if(p.exec()==QDialog::Accepted){
        for(int i=0;i<ui->medicineTableWidget->rowCount();i++){
            ui->medicineTableWidget->removeRow(0);
        }
        ui->diseasePlainTextEdit->clear();
        ui->AddButton->setEnabled(true);
        ui->DelButton->setEnabled(true);
        ui->OKButton->setEnabled(true);
        ui->PrintButton->setEnabled(true);
        QString ID=p.getId();
        patientID=ID;
        qDebug()<<ID.count();
        QString patientId;
        int j=0;
        for(int i=0;i<9;i++){
            if(i<3){
                patientId.insert(i,ID.at(i));
            }
            else if (i<6) {
                patientId.insert(i,'*');
            }
            else{
                patientId.insert(i,ID.at((ID.count()-3)+j));
                j++;
            }
        }
        ui->patientIdLabel->setText(patientId);
        dbManager db;
        db.openDB();
        QSqlQuery Query;
        QString patientSql="select * from patient where ID='"+ID+"';";
        Query.prepare(patientSql);
        Query.exec();
        while(Query.next()){
            QString patientName=Query.value("patient_name").toString();
            QString patientAge=Query.value("patient_age").toString();
            int patientSex=Query.value("patient_Sex").toInt();
            QString patientContact=Query.value("patient_contact").toString();
            QString emergencyContact=Query.value("emergency_contact").toString();
            QString allergyHistory=Query.value("allergy_history").toString();
            ui->patientNameLabel->setText(patientName);
            ui->patientAgeLabel->setText(patientAge);
            if(patientSex==1){
                ui->patientSexLabel->setText("男");
            }
            else if (patientSex==0) {
                ui->patientSexLabel->setText("女");
            }
            ui->patientContactLabel->setText(patientContact);
            ui->emergencyContactLabel->setText(emergencyContact);
            ui->allergyPlainTextEdit->setPlainText(allergyHistory);
        }
        QString pspSql="select * from psp,staff where ID='"+ID+"' and staff.staff_id=psp.doctor_id;";
        Query.prepare(pspSql);
        Query.exec();
        QStringList pspId;
        QStringList patientDisease;
        QStringList pspDate;
        QStringList staffName;
        QDateTime psp[10];
        int i=0;
        while (Query.next()){
            pspId.insert(i,Query.value("psp_id").toString());
            patientDisease.insert(i,Query.value("patient_disease").toString());
            staffName.insert(i,Query.value("staff_name").toString());
            pspDate.insert(i,Query.value("psp_date").toString());
        }
        QDateTime currentTime=QDateTime::currentDateTime();
        for(int i=0;i<pspDate.count();i++){
            qDebug()<<pspDate.at(i);
            QDateTime pspTime;
            pspTime=pspTime.fromString(pspDate.at(i),"yyyy-MM-dd hh:mm:ss");
            if(pspTime>=currentTime.addDays(-7)){
                QString disease=patientDisease.at(i)+" 诊断者:"+staffName.at(i)+" 诊断时间:"+pspTime.toString("yyyy-MM-dd hh:mm:ss dddd")+" 处方单号:"+pspId.at(i);
                QListWidgetItem *Item=new QListWidgetItem();
                Item->setText(disease);
                ui->historyDiseaseListWidget->insertItem(i,Item);
            }
        }
    }
}


void diagnosisWindow::on_AddButton_clicked()
{
    medicineDialog m(this);
    connect(&m,SIGNAL(setMedicineInformation(QString,QString,QString,float,QString)),this,SLOT(getMedicineInformation(QString,QString,QString,float,QString)));
    m.setModal(false);
    m.exec();

}
void diagnosisWindow::getMedicineInformation(QString drugId,QString drugName,QString num,float drugPrice,QString info){
    if(ui->medicineTableWidget->rowCount()!=0){
        bool judge=0;
        for(int i=0;i<ui->medicineTableWidget->rowCount();i++){
            if(ui->medicineTableWidget->item(i,0)->text()==drugId){
                judge=0;
                int num1=ui->medicineTableWidget->item(i,2)->text().toInt();
                ui->medicineTableWidget->item(i,2)->setText(QString::number(num1+num.toInt()));
                break;
            }
            else{
                judge=1;
            }
        }
        if(judge){
            QTableWidgetItem *drugIdItem=new QTableWidgetItem(drugId);
            QTableWidgetItem *drugNameItem=new QTableWidgetItem(drugName);
            QTableWidgetItem *numItem=new QTableWidgetItem(num);
            QTableWidgetItem *drugPriceItem=new QTableWidgetItem(QString::number(drugPrice));
            QTableWidgetItem *infoItem=new QTableWidgetItem(info);
            ui->medicineTableWidget->insertRow(0);
            ui->medicineTableWidget->setItem(0,0,drugIdItem);
            ui->medicineTableWidget->setItem(0,1,drugNameItem);
            ui->medicineTableWidget->setItem(0,2,numItem);
            ui->medicineTableWidget->setItem(0,3,drugPriceItem);
            ui->medicineTableWidget->setItem(0,4,infoItem);
        }
    }
    else{
        QTableWidgetItem *drugIdItem=new QTableWidgetItem(drugId);
        QTableWidgetItem *drugNameItem=new QTableWidgetItem(drugName);
        QTableWidgetItem *numItem=new QTableWidgetItem(num);
        QTableWidgetItem *drugPriceItem=new QTableWidgetItem(QString::number(drugPrice));
        QTableWidgetItem *infoItem=new QTableWidgetItem(info);
        ui->medicineTableWidget->insertRow(0);
        ui->medicineTableWidget->setItem(0,0,drugIdItem);
        ui->medicineTableWidget->setItem(0,1,drugNameItem);
        ui->medicineTableWidget->setItem(0,2,numItem);
        ui->medicineTableWidget->setItem(0,3,drugPriceItem);
        ui->medicineTableWidget->setItem(0,4,infoItem);
    }

}
void diagnosisWindow::on_DelButton_clicked()
{
    ui->medicineTableWidget->removeRow(ui->medicineTableWidget->currentRow());
}

void diagnosisWindow::on_OKButton_clicked()
{
    dbManager db;
    db.openDB();
    if(db.getDB()->transaction()){
        bool succ1,succ2;
        QString pspId=QDateTime::currentDateTime().toString("yyyyMMddhhmmss")+ui->patientIdLabel->text().at(0)+ui->patientIdLabel->text().at(8);
        QString doctorId=ui->StaffIdLabel->text();
        QString patientDisease=ui->diseasePlainTextEdit->toPlainText();
        QString pspTime=QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
        QString pspSql="INSERT INTO psp(psp_id,doctor_id,ID,patient_disease,psp_date) VALUES(?,?,?,?,?);";
        qDebug()<<pspId <<doctorId <<patientID <<patientDisease <<pspTime;
        QSqlQuery query;
        query.prepare(pspSql);
        query.bindValue(0,pspId);
        query.bindValue(1,doctorId);
        query.bindValue(2,patientID);
        query.bindValue(3,patientDisease);
        query.bindValue(4,pspTime);
        succ1=query.exec();
        if(!succ1){
            qDebug()<<query.lastError();
            QMessageBox::critical(this,"Error","执行SQL语句失败，将要回滚");
            if(!db.getDB()->rollback()){
                QMessageBox::critical(this,"Error","回滚失败");
                qDebug()<<db.getDB()->lastError();
            }
            return;
        }
        for(int i=0;i<ui->medicineTableWidget->rowCount();i++){
            QString drugSql="INSERT INTO pspdetail(psp_id,drug_id,drug_num,psp_price,psp_info) VALUES(?,?,?,?,?);";
            QString drugId=ui->medicineTableWidget->item(i,0)->text();
            QString drugNum=ui->medicineTableWidget->item(i,2)->text();
            QString pspPrice=ui->medicineTableWidget->item(i,3)->text();
            QString pspInfo=ui->medicineTableWidget->item(i,4)->text();
            query.prepare(drugSql);
            query.bindValue(0,pspId);
            query.bindValue(1,drugId);
            query.bindValue(2,drugNum);
            query.bindValue(3,pspPrice);
            query.bindValue(4,pspInfo);
            succ2=query.exec();
            if(!succ2){
                qDebug()<<query.lastError();
                QMessageBox::critical(this,"Error","执行SQL语句失败，将要回滚");
                if(!db.getDB()->rollback()){
                    QMessageBox::critical(this,"Error","回滚失败");
                    qDebug()<<db.getDB()->lastError();
                }
                break;
            }
        }
        if(!db.getDB()->commit()){
            QMessageBox::critical(this,"Error","提交失败，将要回滚");
            if(!db.getDB()->rollback()){
                QMessageBox::critical(this,"Error","回滚失败");
            }
        }
        else{
            this->pspID=pspId;
        }
    }
}
QString diagnosisWindow::makePspHtml()
{
    if(!pspID.isEmpty()){
        dbManager db;
        db.openDB();
        QSqlQuery query;
        QString sql="select * from pspdetail where psp_id='"+this->pspID+"';";
        query.exec(sql);
        QString html="<head><h1 style="">缴费单</h1>";
        while (query.next()) {
            QString drugName=query.value("drug_name").toString();
            QString drugNum=query.value("drug_num").toString();
            QString pspPrice=query.value("psp_price").toString();
            QString pspInfo=query.value("psp_info").toString();
        }
    }
}

void diagnosisWindow::on_PrintButton_clicked()
{
    QString html;
    html="<html>";
    html+="<head>";
    html+="<meta http-equiv=Content-Type content=text/html; charset=gb2312 />";
    html+="<STYLE type=text/css>";
    html+="*{margin:0;padding:0;}";
    html+="body, html {";
    html+="margin: 0;";
    html+="padding:0 !important;";
    html+="padding:90px 0 32px 0;";
    html+="width:100%;";
    html+="height:500px;";
    html+="overflow:hidden;}";
    html+=".header {background: #C9F;width: 100%;height: 90px;overflow: hidden;position: absolute;top: 0;width: 100%;text-align: center;background-color: #FFCC00;}";
    html+=".content {position:absolute!important;position:relative;top:90px!important;top:0;bottom:32px;width:100%;overflow:hidden;height:auto!important;height:100%;left: -3px;}";
    html+=".main {height:100%;background:#66CCFF;overflow-y:auto;text-align:center;}";
    html+=".footer {background: #9CF;width: 100%;height: 40px;color: #e1efff;line-height: 32px;letter-spacing: 1px;text-align: center;clear: both;position: absolute;bottom: 0;left: 0;background-color: #FF6600;}</STYLE></head>";
   html+=" <body><div class=\"header\"><br />这里是顶部</div><div class=\"content\"><div class=\"main\"><br /><br /><p>网页header和footer高度是固定的，中间的content高度自适应浏览器窗口高度代码，随着窗口的大小变动都是满屏的。</p><p></p></div></div><div class=\"footer\">这里是底部</div></body></html>";
//------------------------------------------
    QPrintDialog p(this);
    if(p.exec()==QDialog::Accepted){
        QPrinter *print=p.printer();
        QTextDocument text;
        text.setHtml(html);
        text.print(print);
    }
}

void diagnosisWindow::on_historyDiseaseListWidget_itemDoubleClicked(QListWidgetItem *item)
{
    QString disease=item->text();
    QRegExp rxlen("(\\d{16})(?:\\s*)");  //正则表达式取至少16位数字
    int pos = rxlen.indexIn(disease);
    if (pos > -1) {
        QString pspId = rxlen.cap(1); // pspId由16位数字组成
        historyMedicineDialog h(this);
        connect(this,SIGNAL(setPspId(QString)),&h,SLOT(getPspId(QString)));
        emit setPspId(pspId);
        h.setModal(false);
        if(h.exec()==QDialog::Accepted){
            if(ui->medicineTableWidget->rowCount()!=0){
                for(int i=0;i<h.medicineId.count();i++){
                    bool judge=0;
                    for(int j=0;j<ui->medicineTableWidget->rowCount();j++){
                        if(ui->medicineTableWidget->item(j,0)->text()==h.medicineId.at(i)){
                            judge=0;
                            int num1=ui->medicineTableWidget->item(j,2)->text().toInt();
                            ui->medicineTableWidget->item(j,2)->setText(QString::number(num1+h.medicinenum.at(i).toInt()));
                            break;
                        }
                        else{
                            judge=1;
                        }
                    }
                    if(judge){
                        QTableWidgetItem *drugIdItem=new QTableWidgetItem(h.medicineId.at(i));
                        QTableWidgetItem *drugNameItem=new QTableWidgetItem(h.medicineName.at(i));
                        QTableWidgetItem *numItem=new QTableWidgetItem(h.medicinenum.at(i));
                        QTableWidgetItem *drugPriceItem=new QTableWidgetItem(h.medicinePrice.at(i));
                        QTableWidgetItem *infoItem=new QTableWidgetItem(h.medicineinfo.at(i));
                        ui->medicineTableWidget->insertRow(0);
                        ui->medicineTableWidget->setItem(0,0,drugIdItem);
                        ui->medicineTableWidget->setItem(0,1,drugNameItem);
                        ui->medicineTableWidget->setItem(0,2,numItem);
                        ui->medicineTableWidget->setItem(0,3,drugPriceItem);
                        ui->medicineTableWidget->setItem(0,4,infoItem);
                    }
                }
            }
            else{
                for(int i=0;i<h.medicineId.count();i++){
                    QTableWidgetItem *drugIdItem=new QTableWidgetItem(h.medicineId.at(i));
                    QTableWidgetItem *drugNameItem=new QTableWidgetItem(h.medicineName.at(i));
                    QTableWidgetItem *numItem=new QTableWidgetItem(h.medicinenum.at(i));
                    QTableWidgetItem *drugPriceItem=new QTableWidgetItem(h.medicinePrice.at(i));
                    QTableWidgetItem *infoItem=new QTableWidgetItem(h.medicineinfo.at(i));
                    ui->medicineTableWidget->insertRow(0);
                    ui->medicineTableWidget->setItem(0,0,drugIdItem);
                    ui->medicineTableWidget->setItem(0,1,drugNameItem);
                    ui->medicineTableWidget->setItem(0,2,numItem);
                    ui->medicineTableWidget->setItem(0,3,drugPriceItem);
                    ui->medicineTableWidget->setItem(0,4,infoItem);
                }
            }
        }
    }
}

void diagnosisWindow::on_medicineTableWidget_itemChanged(QTableWidgetItem *item)
{
    if(item->column()==2){
        if(ui->medicineTableWidget->item(item->row(),3)!=NULL){
            dbManager db;
            db.openDB();
            int row=item->row();
            QString sql="select * from drug where drug_id ="+ui->medicineTableWidget->item(row,0)->text()+";";
            QSqlQuery query;
            query.exec(sql);
            while(query.next()){
                float price=query.value("drug_price").toFloat();
                int num=item->text().toInt();
                QString drugPrice=QString::number(num*price);
                ui->medicineTableWidget->item(row,3)->setText(drugPrice);
            }
        }
    }

}
