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
        QString pspSql="select * from psp where ID='"+ID+"';";
        Query.prepare(pspSql);
        Query.exec();
        while (Query.next()){
            QString pspId=Query.value("psp_id").toString();
            QString patientDisease=Query.value("patient_disease").toString();
            ui->diseasePlainTextEdit->setPlainText(patientDisease);
        }
    }
}































