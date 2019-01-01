#include "registerwindow.h"
#include "ui_registerwindow.h"

registerWindow::registerWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::registerWindow)
{
    ui->setupUi(this);
}

registerWindow::~registerWindow()
{
    delete ui;
}
void registerWindow::getAccountAndToolTip(int account,int dpmdetail)//account为账号即员工号，dpmdetail为科室详情号
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

void registerWindow::on_registerPushButton_clicked()
{
    QString sql="INSERT INTO patient(patient_name,ID,patient_age,patient_sex,patient_contact,emergency_contact,allergy_history)VALUES(?,?,?,?,?,?,?);";
    dbManager db;
    db.openDB();
    QSqlQuery query;
    query.prepare(sql);
    query.bindValue(0,ui->NameLineEdit->text());
    query.bindValue(1,ui->IDLineEdit->text());
    query.bindValue(2,ui->AgeLineEdit->text());
    if(ui->SexLineEdit->text()=="男"){
        query.bindValue(3,1);
    }else{
        query.bindValue(3,0);
    }
    query.bindValue(4,ui->patientContactLineEdit->text());
    query.bindValue(5,ui->emergencyContactLineEdit->text());
    query.bindValue(6,ui->plainTextEdit->toPlainText());
    bool x=query.exec();
    if(x){
        QMessageBox::information(NULL,"正确","插入成功",QMessageBox::Yes|QMessageBox::No,QMessageBox::Yes);

    }else{
        qDebug()<<query.lastError();

    }
}
