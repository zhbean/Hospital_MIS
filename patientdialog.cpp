#include "patientdialog.h"
#include "ui_patientdialog.h"
#include "QtDebug"
patientDialog::patientDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::patientDialog)
{
    ui->setupUi(this);
    QRegExp rx("^[A-Za-z0-9]+$");
    ui->IDLineEdit->setValidator(new QRegExpValidator(rx,this));
    ui->buttonBox->setEnabled(false);
}

patientDialog::~patientDialog()
{
    delete ui;
}
QString patientDialog::getId(){
    return ID;
}


void patientDialog::on_buttonBox_accepted()
{
    ID=ui->IDLineEdit->text();
}



void patientDialog::on_IDLineEdit_textChanged(const QString &arg1)
{
    QString s=ui->IDLineEdit->text();
    if(s.count()==18||s.count()==15){
        ui->buttonBox->setEnabled(true);
    }
    else{
        ui->buttonBox->setEnabled(false);
    }
}
