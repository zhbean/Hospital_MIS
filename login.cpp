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

void Login::showEvent(QShowEvent *event)
{
    //初始化
    this->ui->comboBox->clear();
    QStandardItemModel *combM = new QStandardItemModel(this);
    QStandardItem *combI;
    dbManager db;
    QSqlQuery query = db.selectDB("select * from dpmdetail;");
    if(query.exec()){
        while(query.next()){
            combI=new QStandardItem(query.value("dpmdetail_name").toString());
            //设置提示
            combI->setToolTip(query.value("dpmdetail_id").toString());
            combM->appendRow(combI);
        }
        this->ui->comboBox->setModel(combM);
    }
    else{
        QMessageBox::information(NULL,"错误","数据连接失败！",QMessageBox::Yes|QMessageBox::No,QMessageBox::Yes);
        qDebug()<<"没有查询结果";
        return;
    }
}


void Login::on_OkButton_clicked()
{
    //获取输入
}
