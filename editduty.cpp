#include "editduty.h"
#include "ui_editduty.h"

EditDuty::EditDuty(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::EditDuty)
{
    ui->setupUi(this);
}

EditDuty::~EditDuty()
{
    delete ui;
}

void EditDuty::showEvent(QShowEvent * event)
{
    initCombobox();
    initLineEdit();
}

//初始化科室下拉框
void EditDuty::initCombobox()
{
    dbManager db;
    if(db.openDB()){
        QString sqlDpm = "select * from department;";
        QString sqlDpmDtl = "select * from dpmdetail;";

        QStandardItemModel * model = new QStandardItemModel();//用来填充下拉框
        QStandardItemModel * dmodel = new QStandardItemModel();//用来填充下拉框
        QStandardItem * item;//用来填充model

        QSqlQuery query;

        if(query.exec(sqlDpm)){
            while(query.next()){
                item = new QStandardItem(query.value("department_name").toString());
                item->setToolTip(query.value("department_id").toString());
                model->appendRow(item);
            }
            this->ui->comb_department->setModel(model);
        }
        else{qDebug()<<"查询科室出错";}

        if(query.exec(sqlDpmDtl)){
            while(query.next()){
                item = new QStandardItem(query.value("dpmdetail_name").toString());
                item->setToolTip(query.value("dpmdetail_id").toString());
                dmodel->appendRow(item);
            }
            this->ui->comb_dpmDetail->setModel(dmodel);
        }
        else{qDebug()<<"查询科室详情出错";}
    }
    else{qDebug()<<"数据库未打开";}
}

void EditDuty::initLineEdit()
{
    dbManager db;
    if(db.openDB()){

        QStringList nameList,idList;
        QString sqlStaff = "select * from staff;";
        QSqlQuery query;

        if(query.exec(sqlStaff)){
            while(query.next()){
                nameList<<query.value("staff_name").toString();
                idList<<query.value("staff_id").toString();
            }

            QCompleter * nameCpl = new QCompleter(nameList,this);
            QCompleter * idCpl = new QCompleter(idList,this);
            this->ui->edt_staffName->setCompleter(nameCpl);
            this->ui->edt_staffID->setCompleter(idCpl);
        }
        else{qDebug()<<"查询员工出错";}

    }
    else{qDebug()<<"数据库未打开";}
}

void EditDuty::setPage(int flag)
{
    switch(flag)
    {
    case 0:
        this->ui->stackedWidget->setCurrentIndex(0);
        this->setWindowTitle("添加值班安排");
        break;
    case 1:
        this->ui->stackedWidget->setCurrentIndex(1);
        break;
    case 2:
        this->ui->stackedWidget->setCurrentIndex(2);
        break;
    default:break;
    }
}

//把拿到id的duty填入控件
void EditDuty::initDuty(QString dutyID)
{
    this->setWindowTitle("值班安排:"+dutyID);
    dbManager db;
    if(db.openDB()){
        QString sqlDuty = "select * from duty,staff,department,dpmdetail where duty.duty_id="+dutyID+" and staff.staff_id=duty.staff_id and duty.dpmdetail_id=dpmdetail.dpmdetail_id and department.department_id=dpmdetail.department_id;";
        QSqlQuery query;

        if(query.exec(sqlDuty)){
            while(query.next()){
                this->ui->comb_department->setCurrentText(query.value("department.department_name").toString());
                this->ui->comb_dpmDetail->setCurrentText(query.value("dpmdetail.dpmdetail_name").toString());
                this->ui->edt_staffID->setText(query.value("staff.staff_id").toString());
                this->ui->edt_staffName->setText(query.value("staff.staff_name").toString());
                this->ui->dte_start->setTime(query.value("duty.start_date").toTime());
                this->ui->dte_end->setTime(query.value("duty.end_date").toTime());
            }
         }
    }
    else{qDebug()<<"数据库未打开";}
}

//添加值班信息
void EditDuty::on_btn_cAddDuty_clicked()
{
    if(this->ui->edt_staffID->text().trimmed().isEmpty()||this->ui->edt_staffName->text().trimmed().isEmpty())
    {
        QMessageBox::information(NULL,"错误","没有输入！",QMessageBox::Yes|QMessageBox::No,QMessageBox::Yes);//错误处理
        return;
    }
    //取科室号
    int curRow=this->ui->comb_dpmDetail->currentIndex();
    QModelIndex index = this->ui->comb_dpmDetail->model()->index(curRow,0);
    QStandardItemModel* model = static_cast<QStandardItemModel*>(this->ui->comb_dpmDetail->model());//拿到combobox里加入的model
    QStandardItem *item = model->itemFromIndex(index);//从model拿到item

    int dpmdetailID = item->toolTip().toInt();

    QString staffID = this->ui->edt_staffID->text();
    QString startDate = this->ui->dte_start->text();
    QString endDate = this->ui->dte_end->text();

    dbManager db;
    if(db.openDB()){
        QString sqlDuty = "insert into duty(staff_id,dpmdetail_id,start_date,end_date) values(?,?,?,?);";
        QSqlQuery query;
        query.prepare(sqlDuty);
        query.addBindValue(staffID);
        query.addBindValue(dpmdetailID);
        query.addBindValue(startDate);
        query.addBindValue(endDate);
        if(query.exec()){
            QMessageBox::information(NULL,"错误","插入成功！",QMessageBox::Yes|QMessageBox::No,QMessageBox::Yes);
            this->close();
        }
        else{qDebug()<<query.lastError();QMessageBox::information(NULL,"错误","插入失败！",QMessageBox::Yes|QMessageBox::No,QMessageBox::Yes);}
    }
    else{QMessageBox::information(NULL,"错误","数据库未打开！",QMessageBox::Yes|QMessageBox::No,QMessageBox::Yes);}
}

void EditDuty::on_btn_cUpdateDuty_clicked()
{

    if(this->ui->edt_staffID->text().trimmed().isEmpty()||this->ui->edt_staffName->text().trimmed().isEmpty())
    {
        QMessageBox::information(NULL,"错误","没有输入！",QMessageBox::Yes|QMessageBox::No,QMessageBox::Yes);//错误处理
        return;
    }
    //取科室号
    int curRow=this->ui->comb_dpmDetail->currentIndex();
    QModelIndex index = this->ui->comb_dpmDetail->model()->index(curRow,0);
    QStandardItemModel* model = static_cast<QStandardItemModel*>(this->ui->comb_dpmDetail->model());//拿到combobox里加入的model
    QStandardItem *item = model->itemFromIndex(index);//从model拿到item

    int dpmdetailID = item->toolTip().toInt();

    QRegExp rx("(?:\\s*)(\\d+)");
    QString dutyID;
    if(rx.indexIn(this->windowTitle())>-1)
        dutyID =rx.cap(1);
    else{QMessageBox::information(NULL,"错误","没有获取到编号！",QMessageBox::Yes|QMessageBox::No,QMessageBox::Yes);}
    QString staffID = this->ui->edt_staffID->text();
    QString startDate = this->ui->dte_start->text();
    QString endDate = this->ui->dte_end->text();

    dbManager db;
    if(db.openDB()){
        QString sqlDuty = "update duty set staff_id=:staffID,dpmdetail_id=:dpmdetailID,start_date=:startDate,end_date=:endDate where duty_id=:dutyID";
        QSqlQuery query;
        query.prepare(sqlDuty);
        query.addBindValue(staffID);
        query.addBindValue(dpmdetailID);
        query.addBindValue(startDate);
        query.addBindValue(endDate);
        query.addBindValue(dutyID);
        if(query.exec()){
            QMessageBox::information(NULL,"恭喜","更新成功！",QMessageBox::Yes|QMessageBox::No,QMessageBox::Yes);
            this->close();
        }
        else{qDebug()<<query.lastError();QMessageBox::information(NULL,"错误","更新失败！",QMessageBox::Yes|QMessageBox::No,QMessageBox::Yes);}
    }
    else{QMessageBox::information(NULL,"错误","数据库未打开！",QMessageBox::Yes|QMessageBox::No,QMessageBox::Yes);}
}

void EditDuty::on_btn_cDelDuty_clicked()
{
    if(this->ui->edt_staffID->text().trimmed().isEmpty()||this->ui->edt_staffName->text().trimmed().isEmpty())
    {
        QMessageBox::information(NULL,"错误","没有输入！",QMessageBox::Yes|QMessageBox::No,QMessageBox::Yes);//错误处理
        return;
    }

    QRegExp rx("(?:\\s*)(\\d+)");
    QString dutyID;
    if(rx.indexIn(this->windowTitle())>-1)
        dutyID =rx.cap(1);
    else{QMessageBox::information(NULL,"错误","没有获取到编号！",QMessageBox::Yes|QMessageBox::No,QMessageBox::Yes);}
    dbManager db;
    if(db.openDB()){
        QString sqlDuty = "delete from duty where duty_id=:dutyID";
        QSqlQuery query;
        query.prepare(sqlDuty);
        query.addBindValue(dutyID);
        if(query.exec()){
            QMessageBox::information(NULL,"恭喜","删除成功！",QMessageBox::Yes|QMessageBox::No,QMessageBox::Yes);
            this->close();
        }
        else{qDebug()<<query.lastError();QMessageBox::information(NULL,"错误","删除失败！",QMessageBox::Yes|QMessageBox::No,QMessageBox::Yes);}
    }
    else{QMessageBox::information(NULL,"错误","数据库未打开！",QMessageBox::Yes|QMessageBox::No,QMessageBox::Yes);}
}
