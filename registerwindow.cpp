#include "registerwindow.h"
#include "ui_registerwindow.h"

registerWindow::registerWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::registerWindow)
{
    ui->setupUi(this);
    ui->tableWidget->setColumnWidth(0,400);
    ui->tableWidget->setColumnWidth(1,140);
    ui->tableWidget->setColumnWidth(2,200);

    QRegExp rx("^[A-Za-z0-9]+$");
    ui->pspIdLineEdit->setValidator(new QRegExpValidator(rx,this));
    ui->IDLineEdit->setValidator(new QRegExpValidator(rx,this));
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

void registerWindow::on_searchPushButton_clicked()
{
    QString sql="select * from pspdetail,drug,psp,patient where psp.ID=patient.ID and psp.psp_id=pspdetail.psp_id and drug.drug_id=pspdetail.drug_id and pspdetail.psp_id='"+ui->pspIdLineEdit->text()+"';";
    dbManager db;
    db.openDB();
    QSqlQuery query;
    query.prepare(sql);
    query.exec(sql);
    double price=0;
    while(query.next()){
        QString drugName=query.value("drug_name").toString();
        QString drugNum=query.value("drug_num").toString();
        QString drugprice=query.value("psp_price").toString();
        QString patientName=query.value("patient_name").toString();
        price+=query.value("psp_price").toDouble();
        ui->tableWidget->insertRow(0);
        QTableWidgetItem *drugNameItem=new QTableWidgetItem(drugName);
        ui->tableWidget->setItem(0,0,drugNameItem);
        QTableWidgetItem *drugNumItem=new QTableWidgetItem(drugNum);
        ui->tableWidget->setItem(0,1,drugNumItem);
        QTableWidgetItem *drugpriceItem=new QTableWidgetItem(drugprice);
        ui->tableWidget->setItem(0,2,drugpriceItem);
        ui->patientNameLabel->setText(patientName);
    }
    ui->payLabel->setNum(price);
}

void registerWindow::on_PayPushButton_clicked()
{
}

void registerWindow::on_IDLineEdit_textChanged(const QString &arg1)
{
    QString s=ui->IDLineEdit->text();
    if(s.count()==18||s.count()==15){
        ui->registerPushButton->setEnabled(true);
    }
    else{
        ui->registerPushButton->setEnabled(false);
    }
}
