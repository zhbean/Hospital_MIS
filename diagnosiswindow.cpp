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
        ui->AddButton->setEnabled(true);
        ui->DelButton->setEnabled(true);
        ui->OKButton->setEnabled(true);
        ui->PrintButton->setEnabled(true);
        QString ID=p.getId();
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
void diagnosisWindow::on_DelButton_clicked()
{
    ui->medicineTableWidget->removeRow(ui->medicineTableWidget->currentRow());
}

void diagnosisWindow::on_OKButton_clicked()
{

}

void diagnosisWindow::on_PrintButton_clicked(){


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
