#include "managewindow.h"
#include "ui_managewindow.h"

manageWindow::manageWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::manageWindow)
{
    ui->setupUi(this);

    ui->menubar->addAction("基本设置",this,SLOT(showSetting()));//自己添加一个菜单
    ui->menubar->addAction("门诊统计",this,SLOT(showStatistics()));
}

manageWindow::~manageWindow()
{
    delete ui;
}

void manageWindow::getAccountAndToolTip(int account,int dpmdetail)//account为账号即员工号，dpmdetail为科室详情号
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

void manageWindow::showSetting()
{
    ui->stackedWidget->setCurrentIndex(1);
}

void manageWindow::showStatistics()
{
    ui->stackedWidget->setCurrentIndex(0);
}

void manageWindow::showEvent(QShowEvent *event)
{
    initDepartment();
}

//初始化科室信息
void manageWindow::initDepartment()
{
    QStandardItemModel* pModel = new QStandardItemModel(ui->trv_department);//取父节点
    pModel->setHorizontalHeaderLabels(QStringList()<<"序号"<<"科室名"<<"房间名"<<"信息"<<"房间");//设置标题

    dbManager db;
    if(db.openDB()){
        QSqlQuery qDpm;
        if(qDpm.exec("select * from department;")){
            while(qDpm.next()){
                QString dpm_id = qDpm.value("department_id").toString();
                QString dpm = qDpm.value("department_name").toString();
                QString dpm_info = qDpm.value("department_info").toString();
                QStandardItem * pItem = new QStandardItem(dpm_id);//给model赋第一列值
                pModel->appendRow(pItem);
                pModel->setItem(pModel->indexFromItem(pItem).row(),1, new QStandardItem(dpm));//给model赋第二列值
                pModel->setItem(pModel->indexFromItem(pItem).row(),3, new QStandardItem(dpm_info));//给model赋第二列值

                QSqlQuery qDpmDtl;
                if(qDpmDtl.exec("select * from dpmdetail where department_id = "+dpm_id+";")){
                    while(qDpmDtl.next()){
                        QString dpmDtl_id = qDpmDtl.value("dpmdetail_id").toString();
                        QString dpmDtl = qDpmDtl.value("dpmdetail_name").toString();
                        QString dpmDtl_position = qDpmDtl.value("dpmdetail_position").toString();
                        QStandardItem * cItem = new QStandardItem(dpmDtl_id);//给model赋第一列值
                        pItem->appendRow(cItem);
                        pItem->setChild(cItem->index().row(),2, new QStandardItem(dpmDtl));//给model赋第二列值
                        pItem->setChild(cItem->index().row(),4, new QStandardItem(dpmDtl_position));//给model赋第二列值
                    }
                }
                else{qDebug()<<"详情数据查询出错";}
            }
            ui->trv_department->setModel(pModel);
            ui->trv_department->expandAll();
        }
        else{qDebug()<<"数据查询出错";}
    }
    else{qDebug()<<"数据库未连接";}
}

void manageWindow::on_trv_department_clicked(const QModelIndex &index)
{
    QModelIndex parent= index.parent();
    int curRow = index.row();
    int prow = parent.isValid();//有父节点为科室详情，无父节点为科室
    //QStandardItemModel* model = static_cast<QStandardItemModel*>(ui->trv_department->model());//拿到treeView里加入的model
    //QStandardItem *item = model->itemFromIndex(index);//从model拿到item
    if(prow)
    {
        ui->stackedWidget_2->setCurrentIndex(1);
        ui->edt_dpmID_3->setText(parent.sibling(parent.row(),0).data().toString());
        ui->edt_dpmID_2->setText(index.sibling(curRow,0).data().toString());
        ui->edt_dpmName_2->setText(index.sibling(curRow,2).data().toString());
        ui->edt_dpmInfo_2->setText(index.sibling(curRow,4).data().toString());
    }
    else
    {
        ui->stackedWidget_2->setCurrentIndex(0);
        ui->edt_dpmID->setText(index.sibling(curRow,0).data().toString());
        ui->edt_dpmName->setText(index.sibling(curRow,1).data().toString());
        ui->edt_dpmInfo->setText(index.sibling(curRow,3).data().toString());
    }
}

void manageWindow::on_btn_addDpm_clicked()
{
    dbManager db;
    if(db.openDB()){
        QString dpmName = ui->edt_dpmName->text();
        QString dpmInfo = ui->edt_dpmInfo->text();
        QString sql = "insert into department(department_name,department_info) values(?,?);";
        QSqlQuery query;
        query.prepare(sql);
        query.addBindValue(dpmName);
        query.addBindValue(dpmInfo);
        if(query.exec()){
            if(query.numRowsAffected()==0){
                qDebug()<<"已有该数据";
            }
            else{qDebug()<<"插入成功";initDepartment();}
        }
        else{qDebug()<<"插入失败";}
    }
    else{qDebug()<<"数据库未开启";}
}

void manageWindow::on_btn_addDpm_2_clicked()
{
    dbManager db;
    if(db.openDB()){
        int dpmID = ui->edt_dpmID->text().toInt();
        QString dpmDtlName = ui->edt_dpmName_2->text();
        QString dpmDtlPosition = ui->edt_dpmInfo_2->text();
        QString sql = "insert into dpmdetail(department_id,dpmdetail_name,dpmdetail_position) values(?,?,?);";
        QSqlQuery query;
        query.prepare(sql);
        query.addBindValue(dpmID);
        query.addBindValue(dpmDtlName);
        query.addBindValue(dpmDtlPosition);
        if(query.exec()){
            if(query.numRowsAffected()==0){
                qDebug()<<"已有该数据";
            }
            else{qDebug()<<"插入成功";initDepartment();}
        }
        else{qDebug()<<"插入失败";}
    }
    else{qDebug()<<"数据库未开启";}
}

void manageWindow::on_btn_addDtl_clicked()
{
    ui->stackedWidget_2->setCurrentIndex(1);
    ui->edt_dpmID_3->setText(ui->edt_dpmID->text());
    ui->edt_dpmID_2->clear();
    ui->edt_dpmName_2->clear();
    ui->edt_dpmInfo_2->clear();
}

void manageWindow::on_btn_updateDpm_2_clicked()
{
    dbManager db;
    if(db.openDB()){
        QString dpmID = ui->edt_dpmID_3->text();
        QString dpmDtlID = ui->edt_dpmID_2->text();
        QString dpmDtlName = ui->edt_dpmName_2->text();
        QString dpmDtlPosition = ui->edt_dpmInfo_2->text();

        QString sql = "update dpmdetail set department_id=:dpmID,dpmdetail_name=:dpmDtlName,dpmdetail_position=:dpmDtlPosition where dpmdetail_id=:dpmDtlID;";
        QSqlQuery query;
        query.prepare(sql);
        query.bindValue(":dpmID",dpmID);
        query.bindValue(":dpmDtlName",dpmDtlName);
        query.bindValue(":dpmDtlPosition",dpmDtlPosition);
        query.bindValue(":dpmDtlID",dpmDtlID);
        if(query.exec()){
            if(query.numRowsAffected()==0){
                qDebug()<<"数据失败";
            }
            else{qDebug()<<"更新成功";initDepartment();}
        }
        else{qDebug()<<query.lastError()<<"执行更新失败";}
    }
    else{qDebug()<<"数据库未开启";}
}

void manageWindow::on_btn_delDpm_2_clicked()
{
    dbManager db;
    if(db.openDB()){
        QString dpmDtlID = ui->edt_dpmID_2->text();

        QString sql = "delete from dpmdetail where dpmdetail_id=:dpmDtlID;";
        QSqlQuery query;
        query.prepare(sql);
        query.bindValue(":dpmDtlID",dpmDtlID);
        if(query.exec()){
            if(query.numRowsAffected()==0){
                qDebug()<<"数据失败";
            }
            else{qDebug()<<"删除成功";initDepartment();}
        }
        else{qDebug()<<query.lastError()<<"执行删除失败";QMessageBox::information(NULL,"错误","有关联关系，无法删除！",QMessageBox::Yes|QMessageBox::No,QMessageBox::Yes);}
    }
    else{qDebug()<<"数据库未开启";}
}

void manageWindow::on_btn_updateDpm_clicked()
{
    dbManager db;
    if(db.openDB()){
        QString dpmName = ui->edt_dpmName->text();
        QString dpmInfo = ui->edt_dpmInfo->text();
        int dpmID = ui->edt_dpmID->text().toInt();

        QString sql = "update department set department_name=:dpmName,department_info=:dpmInfo where department_id=:dpmID;";
        QSqlQuery query;
        query.prepare(sql);
        query.bindValue(":dpmName",dpmName);
        query.bindValue(":dpmInfo",dpmInfo);
        query.bindValue(":dpmID",dpmID);

        if(query.exec()){
            if(query.numRowsAffected()==0){
                qDebug()<<"数据失败";
            }
            else{qDebug()<<"更新成功";initDepartment();}
        }
        else{qDebug()<<query.lastError()<<"执行更新失败";}
    }
    else{qDebug()<<"数据库未开启";}
}

void manageWindow::on_btn_delDpm_clicked()
{
    dbManager db;
    if(db.openDB()){
        int dpmID = ui->edt_dpmID->text().toInt();

        QString sql = "delete from department where department_id=:dpmID;";
        QSqlQuery query;
        query.prepare(sql);
        query.bindValue(":dpmID",dpmID);
        if(query.exec()){
            if(query.numRowsAffected()==0){
                qDebug()<<"数据失败";
            }
            else{qDebug()<<"删除成功";initDepartment();}
        }
        else{qDebug()<<query.lastError()<<"执行删除失败";QMessageBox::information(NULL,"错误","有关联关系，无法删除！",QMessageBox::Yes|QMessageBox::No,QMessageBox::Yes);}
    }
    else{qDebug()<<"数据库未开启";}
}

//初始化员工表格
void manageWindow::initStaff()
{
    dbManager db;
    if(db.openDB()){
        //初始化
        QString sql = "select staff.staff_id,staff.staff_name,staff.staff_sex,staff.staff_age,positions.position_name,positions.position_limit,department.department_name,staff_password from staff,positions,department";
        sql += " where staff.position_id=positions.position_id and staff.department_id=department.department_id;";

        QSqlQueryModel * model = new QSqlQueryModel(ui->tbv_staff);
        model->setQuery(sql);

        model->setHeaderData(0,Qt::Horizontal,QObject::tr("员工编号"));
        model->setHeaderData(1,Qt::Horizontal,QObject::tr("员工姓名"));
        model->setHeaderData(2,Qt::Horizontal,QObject::tr("性别"));
        model->setHeaderData(3,Qt::Horizontal,QObject::tr("年龄"));
        model->setHeaderData(4,Qt::Horizontal,QObject::tr("职位"));
        model->setHeaderData(5,Qt::Horizontal,QObject::tr("职位权限"));
        model->setHeaderData(6,Qt::Horizontal,QObject::tr("所属科室"));
        model->setHeaderData(7,Qt::Horizontal,QObject::tr("账号密码"));

        while(model->canFetchMore())
        {
            model->fetchMore();
        }

        ui->tbv_staff->setModel(model);
        ui->tbv_staff->show();

        ui->tbv_staff->setSortingEnabled(true);

    }
    else{QMessageBox::information(NULL,"错误","数据库未开启！",QMessageBox::Yes|QMessageBox::No,QMessageBox::Yes);}

}
//初始化职工下拉框
void manageWindow::initStaffCombobox()
{
    this->ui->comb_staffPosition->clear();
    this->ui->comb_staffDepartment->clear();

    dbManager db;
    if(db.openDB()){
        QSqlQuery qPosition,qDepartment;
        QString strPosition = "select * from positions;";
        QString strDepartment = "select * from department;";

        if(qPosition.exec(strPosition)){
            QStandardItemModel * model = new QStandardItemModel(this);
            QStandardItem * item;
            while(qPosition.next()){
                item = new QStandardItem(qPosition.value("position_name").toString());
                item->setToolTip("职位限制:"+qPosition.value("position_limit").toString());
                model->appendRow(item);
            }
            this->ui->comb_staffPosition->setModel(model);
        }
        else{QMessageBox::information(NULL,"错误","数据库查询出错！",QMessageBox::Yes|QMessageBox::No,QMessageBox::Yes);}

        if(qDepartment.exec(strDepartment)){
            while(qDepartment.next()){
                this->ui->comb_staffDepartment->addItem(qDepartment.value("department_name").toString());

            }
        }
        else{QMessageBox::information(NULL,"错误","数据库查询出错！",QMessageBox::Yes|QMessageBox::No,QMessageBox::Yes);}
    }
    else{QMessageBox::information(NULL,"错误","数据库未开启！",QMessageBox::Yes|QMessageBox::No,QMessageBox::Yes);}
}


//初始化职位表格
void manageWindow::initPosition()
{
    QSqlRelationalTableModel *model = new QSqlRelationalTableModel();
    model->setTable("positions");
    model->setHeaderData(0,Qt::Horizontal, QObject::tr("职位ID"));
    model->setHeaderData(1,Qt::Horizontal, QObject::tr("职位名"));
    model->setHeaderData(2,Qt::Horizontal, QObject::tr("登陆限制"));
    this->ui->tbv_position->setModel(model);
    model->select();
    //设置自动排序
    ui->tbv_position->setSortingEnabled(true);
}

//页签改变初始化页面
void manageWindow::on_tabWidget_currentChanged(int index)
{
    if(index==2){initStaff();initStaffCombobox();}
    else if(index==3){initPosition();}
}

void manageWindow::on_tbv_position_clicked(const QModelIndex &index)
{
    QString positionID,positionName;
    int positionLimit;
    int curRow = index.row();
    positionID = index.sibling(curRow,0).data().toString();
    positionName = index.sibling(curRow,1).data().toString();
    positionLimit = index.sibling(curRow,2).data().toInt();

    this->ui->edt_positionID->setText(positionID);
    this->ui->edt_positionName->setText(positionName);
    this->ui->comb_positionLimit->setCurrentIndex(positionLimit);
}

void manageWindow::on_btn_addPosition_clicked()
{
    dbManager db;
    if(db.openDB()){
        QString positionName = this->ui->edt_positionName->text();
        int positionLimit = this->ui->comb_positionLimit->currentIndex();

        QString sql = "insert into positions(position_name,position_limit) values(?,?);";
        QSqlQuery query;
        query.prepare(sql);
        query.addBindValue(positionName);
        query.addBindValue(positionLimit);
        if(query.exec()){
            if(query.numRowsAffected()==0){
                qDebug()<<"数据失败";
            }
            else{qDebug()<<"插入成功";}
        }
        else{qDebug()<<query.lastError()<<"执行插入失败";}
    }
    else{qDebug()<<"数据库未开启";}
    initPosition();
}

void manageWindow::on_btn_updatePosition_clicked()
{
    dbManager db;
    if(db.openDB()){
        QString positionName = this->ui->edt_positionName->text();
        int positionLimit = this->ui->comb_positionLimit->currentIndex();
        int positionID = this->ui->edt_positionID->text().toInt();

        QString sql = "update positions set position_name=:positionName,position_limit=:positionLimit where position_id=:positionID;";
        QSqlQuery query;
        query.prepare(sql);
        query.bindValue(":positionName",positionName);
        query.bindValue(":positionLimit",positionLimit);
        query.bindValue(":positionID",positionID);
        if(query.exec()){
            if(query.numRowsAffected()==0){
                qDebug()<<"数据失败";
            }
            else{qDebug()<<"更新成功";}
        }
        else{qDebug()<<query.lastError()<<"执行更新失败";}
    }
    else{qDebug()<<"数据库未开启";}
    initPosition();
}

void manageWindow::on_btn_delPosition_clicked()
{
    dbManager db;
    if(db.openDB()){
        int positionID = this->ui->edt_positionID->text().toInt();

        QString sql = "delete from positions where position_id=:positionID;";
        QSqlQuery query;
        query.prepare(sql);
        query.bindValue(":positionID",positionID);
        if(query.exec()){
            if(query.numRowsAffected()==0){
                qDebug()<<"数据失败";
            }
            else{qDebug()<<"删除成功";}
        }
        else{qDebug()<<query.lastError()<<"执行删除失败";QMessageBox::information(NULL,"错误","有关联关系，无法删除！",QMessageBox::Yes|QMessageBox::No,QMessageBox::Yes);}
    }
    else{qDebug()<<"数据库未开启";}
    initPosition();
}

void manageWindow::on_tbv_staff_clicked(const QModelIndex &index)
{
    int curRow = index.row();
    this->ui->edt_staffName->setText(index.sibling(curRow,1).data().toString());
    this->ui->comb_staffSex->setCurrentIndex(index.sibling(curRow,2).data().toInt());
    this->ui->spb_staffAge->setValue(index.sibling(curRow,3).data().toInt());
    this->ui->comb_staffPosition->setCurrentText(index.sibling(curRow,4).data().toString());
    this->ui->comb_staffDepartment->setCurrentText(index.sibling(curRow,5).data().toString());
    this->ui->edt_staffPassword->setText(index.sibling(curRow,6).data().toString());
}
