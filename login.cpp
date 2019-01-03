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
    else{
        QMessageBox::information(NULL,"错误","数据库错误!",QMessageBox::Yes|QMessageBox::No,QMessageBox::Yes);
        return;
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
                    checkDuty(dpmdetail,account);

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
    this->accept();
    this->judge=1;
}

void Login::checkDuty(int dpmdetail,int account)
{
    //得到当前时间
    QDateTime curTime = QDateTime::currentDateTime();
    curTime.setDate(QDate::fromString(curTime.toString("dddd"),"dddd"));
    dbManager db;
    if(db.openDB()){
/*
        QSqlQuery iq;
        iq.prepare("INSERT INTO duty(staff_id,dpmdetail_id,start_date,end_date) VALUES(10001,1001,'"+curTime.toString("yyyy-MM-dd hh:mm:ss dddd")+"','"+(curTime.addSecs(60*60*24*30)).toString("yyyy-MM-dd hh:mm:ss dddd")+"');");
        iq.exec();
*/
        QSqlQuery query = db.selectDB("select * from duty where dpmdetail_id=? and staff_id=?;");
        query.addBindValue(dpmdetail);
        query.addBindValue(account);
        if(query.exec()){
            if(query.next()){
                do{
                //比对值班表时间与当前时间
                QDateTime startTime;
                QDateTime endTime;
                QString s=query.value("start_date").toString();
                startTime=startTime.fromString(query.value("start_date").toString(),"hh:mm:ss dddd");
                endTime=endTime.fromString(query.value("end_date").toString(),"hh:mm:ss dddd");
                if(startTime.isValid()&&endTime.isValid()){
                    int staff = query.value("staff_id").toInt();
                    int dpm = query.value("dpmdetail_id").toInt();
                    int dutyID = query.value("duty_id").toInt();

                    if(curTime>startTime&&curTime<endTime){
                        writeDuty(dutyID,0,staff,curTime);
                        qDebug()<<"到勤";
                        break;
                    }
                    //else {qDebug()<<"有安排值班但是时间不对替班1";}
                }
                else {
                    qDebug()<<"数据库时间格式有误";
                }
                }while(query.next());
            }
            else{
                qDebug()<<"没有安排值班的替班2";
            }
        }
    }
}

void Login::writeDuty(int dutyID,int status,int staffID,QDateTime curTime)
{
    curTime=curTime.currentDateTime();
    dbManager db;
    if(db.openDB()){
        QSqlQuery query;
        query.prepare("INSERT INTO dutyrecord(duty_id,dutyrecord_status,staff_id,real_start_date,real_end_date) VALUES(?,?,?,?,?);");
        query.addBindValue(dutyID);
        query.addBindValue(status);
        query.addBindValue(staffID);
        query.addBindValue(curTime.toString("yyyy-MM-dd hh:mm:ss dddd"));
        query.addBindValue(curTime.toString("yyyy-MM-dd hh:mm:ss dddd"));
        if(query.exec()){

        }
        else{ qDebug()<<"登录失败";}
    }
}
