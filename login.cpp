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
    if(db.openDB())
    {
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

}


void Login::on_OkButton_clicked()
{
    int position_id;

    //获取帐号密码
    int account = this->ui->AccountEdit->text().toInt();
    QString pwd = this->ui->PassWordEdit->text();
    //科室号toolTip
    int curRow=this->ui->comboBox->currentIndex();
    QModelIndex index = this->ui->comboBox->model()->index(curRow,0);
    QStandardItemModel* model = static_cast<QStandardItemModel*>(ui->comboBox->model());//拿到combobox里加入的model
    QStandardItem *item = model->itemFromIndex(index);//从model拿到item
    int dpmdetail = item->toolTip().toInt();

    //查询数据库鉴别权限
    dbManager db;
    if(db.openDB()){
        QSqlQuery query = db.selectDB("select * from staff where staff_id=? and staff_password=?;");
        query.addBindValue(account);
        query.addBindValue(pwd);
        if(query.exec()){
            if(query.next()){
                //得到职位号
                position_id = query.value("position_id").toInt();
                qDebug()<<position_id;
                //得到权限
                QSqlQuery limit_query = db.selectDB("select * from positions where position_id=?");
                limit_query.addBindValue(position_id);
                if(limit_query.exec()){
                    while(limit_query.next()){
                        limit = limit_query.value("position_limit").toInt();
                    }
                    emit setAccountAndToolTip(account,dpmdetail);
                    this->accept();
                }
                else{
                    QMessageBox::information(NULL,"错误","数据库错误!",QMessageBox::Yes|QMessageBox::No,QMessageBox::Yes);
                    return;
                }
            }
            else{
                //账号或密码错误
                QMessageBox::information(NULL,"错误","账号或密码错误!",QMessageBox::Yes|QMessageBox::No,QMessageBox::Yes);
                return;
            }
        }
        else{
            QMessageBox::information(NULL,"错误","数据库错误!",QMessageBox::Yes|QMessageBox::No,QMessageBox::Yes);
            return;
        }
    }
    else{
        QMessageBox::information(NULL,"错误","数据库错误!",QMessageBox::Yes|QMessageBox::No,QMessageBox::Yes);
        return;
    }
}

void Login::on_CancelButton_clicked()
{
    this->close();
}
