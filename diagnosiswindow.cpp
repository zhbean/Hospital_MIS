#include "diagnosiswindow.h"
#include "ui_diagnosiswindow.h"
#include "patientdialog.h"
#include <QRegExp>
#include <QRegExpValidator>

diagnosisWindow::diagnosisWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::diagnosisWindow)
{
    ui->setupUi(this);
    ui->menubar->addAction("病人刷卡",this,SLOT(showStatistics()));

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
            QString patientSex=Query.value("patient_age").toString();
            QString patientContact=Query.value("patient_contact").toString();
            QString emergencyContact=Query.value("emergency_contact").toString();
            QString allergyHistory=Query.value("allergy_history").toString();
            ui->patientNameLabel->setText(patientName);
            ui->patientAgeLabel->setText(patientAge);
            ui->patientSexLabel->setText(patientSex);
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
                QString disease=patientDisease.at(i)+" 诊断者:"+staffName.at(i)+" 诊断时间:"+pspTime.toString("yyyy-MM-dd hh:mm:ss dddd");
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
    connect(&m,SIGNAL(setMedicineInformation(QString,QString,QString,QString)),this,SLOT(getMedicineInformation(QString,QString,QString,QString)));
    m.setModal(false);
    m.exec();

}
void diagnosisWindow::getMedicineInformation(QString drugId,QString drugName,QString drugPrice,QString info){
    QString medicine="药品名称:"+drugName+" 药品价格:"+drugPrice+" 使用说明:"+info;
    QListWidgetItem *Item=new QListWidgetItem(medicine);
    int row=ui->medicineListWidget->count();
    ui->medicineListWidget->insertItem(row,Item);
}
void diagnosisWindow::on_DelButton_clicked()
{
    QListWidgetItem *Item=ui->medicineListWidget->currentItem();
    delete  Item;
}

void diagnosisWindow::on_OKButton_clicked()
{

}

void diagnosisWindow::on_PrintButton_clicked()
{

}
