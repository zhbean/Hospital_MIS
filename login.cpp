#include "login.h"
#include "ui_login.h"

Login::Login(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Login)
{
    ui->setupUi(this);


    QTimer *timer = new QTimer(this);
       connect(timer,SIGNAL(timeout()),this,SLOT(timerUpdate()));
       timer->start(1000);
}

Login::~Login()
{
    delete ui;
}

void Login::timerUpdate(void)
{
    QDateTime time = QDateTime::currentDateTime();
    QString str = time.toString("yyyy-MM-dd hh:mm:ss dddd");
    ui->Timelabel->setText(str);
}

