#include "managewindow.h"
#include "ui_managewindow.h"

manageWindow::manageWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::manageWindow)
{
    ui->setupUi(this);

    ui->menubar->addAction("基本设置",this,SLOT(showSetting()));//自己添加一个菜单
    ui->menubar->addAction("门诊统计",this,SLOT(showStatistics()));
    ui->menubar->addAction("注销",this,SLOT(signout()));
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
    this->ui->stackedWidget_3->setCurrentIndex(0);
    initDpmStatistics();
}

void manageWindow::signout()
{
    Login l;
    this->close();
    l.exec();
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
            ui->trv_department->setSortingEnabled(true);
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
    if(ui->edt_dpmName->text().trimmed().isEmpty()||ui->edt_dpmInfo->text().trimmed().isEmpty())
    {
        QMessageBox::information(NULL,"错误","请填写科室信息！",QMessageBox::Yes|QMessageBox::No,QMessageBox::Yes);
        return;
    }
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
    if(ui->edt_dpmName_2->text().trimmed().isEmpty()||ui->edt_dpmInfo_2->text().trimmed().isEmpty())
    {
        QMessageBox::information(NULL,"错误","填写科室详情！",QMessageBox::Yes|QMessageBox::No,QMessageBox::Yes);
        return;
    }
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
    if(ui->edt_dpmID->text().trimmed().isEmpty())
    {
        QMessageBox::information(NULL,"错误","选择一个科室！",QMessageBox::Yes|QMessageBox::No,QMessageBox::Yes);
        return;
    }
    ui->stackedWidget_2->setCurrentIndex(1);
    ui->edt_dpmID_3->setText(ui->edt_dpmID->text());
    ui->edt_dpmID_2->clear();
    ui->edt_dpmName_2->clear();
    ui->edt_dpmInfo_2->clear();
}

void manageWindow::on_btn_updateDpm_2_clicked()
{
    if(ui->edt_dpmID_2->text().trimmed().isEmpty())
    {
        QMessageBox::information(NULL,"错误","选择一个科室详情！",QMessageBox::Yes|QMessageBox::No,QMessageBox::Yes);
        return;
    }
    if(ui->edt_dpmID_3->text().trimmed().isEmpty()||ui->edt_dpmName_2->text().trimmed().isEmpty()||ui->edt_dpmInfo_2->text().trimmed().isEmpty())
    {
        QMessageBox::information(NULL,"错误","选择填写科室详情！",QMessageBox::Yes|QMessageBox::No,QMessageBox::Yes);
        return;
    }
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
    if(ui->edt_dpmID_2->text().trimmed().isEmpty())
    {
        QMessageBox::information(NULL,"错误","选择一个科室详情！",QMessageBox::Yes|QMessageBox::No,QMessageBox::Yes);
        return;
    }
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
    if(ui->edt_dpmID->text().trimmed().isEmpty())
    {
        QMessageBox::information(NULL,"错误","选择一个科室！",QMessageBox::Yes|QMessageBox::No,QMessageBox::Yes);
        return;
    }
    if(ui->edt_dpmName->text().trimmed().isEmpty()||ui->edt_dpmInfo->text().trimmed().isEmpty())
    {
        QMessageBox::information(NULL,"错误","请填写科室信息！",QMessageBox::Yes|QMessageBox::No,QMessageBox::Yes);
        return;
    }
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
    if(ui->edt_dpmID->text().trimmed().isEmpty())
    {
        QMessageBox::information(NULL,"错误","选择一个科室！",QMessageBox::Yes|QMessageBox::No,QMessageBox::Yes);
        return;
    }
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
                item->setAccessibleText(qPosition.value("position_id").toString());
                item->setToolTip("登陆限制:"+qPosition.value("position_limit").toString());
                model->appendRow(item);
            }
            this->ui->comb_staffPosition->setModel(model);
        }
        else{QMessageBox::information(NULL,"错误","数据库查询出错！",QMessageBox::Yes|QMessageBox::No,QMessageBox::Yes);}

        if(qDepartment.exec(strDepartment)){
            QStandardItemModel * model = new QStandardItemModel(this);
            QStandardItem * item;
            while(qDepartment.next()){
                item = new QStandardItem(qDepartment.value("department_name").toString());
                item->setToolTip(qDepartment.value("department_id").toString());
                model->appendRow(item);
            }
            this->ui->comb_staffDepartment->setModel(model);
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
    if(index==1){initDuty();}
    else if(index==2){initStaff();initStaffCombobox();}
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
    if(ui->edt_positionName->text().trimmed().isEmpty())
    {
        QMessageBox::information(NULL,"错误","请填写职位信息！",QMessageBox::Yes|QMessageBox::No,QMessageBox::Yes);
        return;
    }
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
    if(ui->edt_positionID->text().trimmed().isEmpty())
    {
        QMessageBox::information(NULL,"错误","请选中职位信息！",QMessageBox::Yes|QMessageBox::No,QMessageBox::Yes);
        return;
    }
    if(ui->edt_positionName->text().trimmed().isEmpty())
    {
        QMessageBox::information(NULL,"错误","请填写职位信息！",QMessageBox::Yes|QMessageBox::No,QMessageBox::Yes);
        return;
    }
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
    if(ui->edt_positionID->text().trimmed().isEmpty())
    {
        QMessageBox::information(NULL,"错误","请选中职位信息！",QMessageBox::Yes|QMessageBox::No,QMessageBox::Yes);
        return;
    }
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
    this->ui->edt_staffID->setText(index.sibling(curRow,0).data().toString());
    this->ui->edt_staffName->setText(index.sibling(curRow,1).data().toString());
    this->ui->comb_staffSex->setCurrentIndex(index.sibling(curRow,2).data().toInt());
    this->ui->spb_staffAge->setValue(index.sibling(curRow,3).data().toInt());
    this->ui->comb_staffPosition->setCurrentText(index.sibling(curRow,4).data().toString());
    this->ui->comb_staffDepartment->setCurrentText(index.sibling(curRow,6).data().toString());
    this->ui->edt_staffPassword->setText(index.sibling(curRow,7).data().toString());
}

void manageWindow::on_btn_addStaff_clicked()
{
    if(ui->edt_staffName->text().trimmed().isEmpty()||ui->edt_staffPassword->text().trimmed().isEmpty())
    {
        QMessageBox::information(NULL,"错误","请填写员工信息！",QMessageBox::Yes|QMessageBox::No,QMessageBox::Yes);
        return;
    }
    QString staffName = this->ui->edt_staffName->text();
    int staffSex = this->ui->comb_staffSex->currentIndex();
    int staffAge = this->ui->spb_staffAge->value();
    //取职位号
    int curRow=this->ui->comb_staffPosition->currentIndex();
    QModelIndex index = this->ui->comb_staffPosition->model()->index(curRow,0);
    QStandardItemModel* model = static_cast<QStandardItemModel*>(ui->comb_staffPosition->model());//拿到combobox里加入的model
    QStandardItem *item = model->itemFromIndex(index);//从model拿到item
    int positionID = item->accessibleText().toInt();
    //取科室号
    curRow=this->ui->comb_staffDepartment->currentIndex();
    index = this->ui->comb_staffDepartment->model()->index(curRow,0);
    model = static_cast<QStandardItemModel*>(ui->comb_staffDepartment->model());//拿到combobox里加入的model
    item = model->itemFromIndex(index);//从model拿到item
    int departmentID = item->toolTip().toInt();

    QString staffPassword = this->ui->edt_staffPassword->text();

    dbManager db;
    if(db.openDB()){

        QString sql = "insert into staff(staff_name,staff_sex,staff_age,position_id,department_id,staff_password) values(?,?,?,?,?,?);";
        QSqlQuery query;
        query.prepare(sql);
        query.addBindValue(staffName);
        query.addBindValue(staffSex);
        query.addBindValue(staffAge);
        query.addBindValue(positionID);
        query.addBindValue(departmentID);
        query.addBindValue(staffPassword);
        if(query.exec()){
            if(query.numRowsAffected()==0){
                qDebug()<<"数据失败";
            }
            else{qDebug()<<"插入成功";}
        }
        else{qDebug()<<query.lastError()<<"执行插入失败";}
    }
    else{qDebug()<<"数据库未开启";}
    initStaff();
}

void manageWindow::on_btn_updateStaff_clicked()
{
    if(ui->edt_staffID->text().trimmed().isEmpty())
    {
        QMessageBox::information(NULL,"错误","请选中员工信息！",QMessageBox::Yes|QMessageBox::No,QMessageBox::Yes);
        return;
    }
    if(ui->edt_staffName->text().trimmed().isEmpty()||ui->edt_staffPassword->text().trimmed().isEmpty())
    {
        QMessageBox::information(NULL,"错误","请填写员工信息！",QMessageBox::Yes|QMessageBox::No,QMessageBox::Yes);
        return;
    }
    int staffID= this->ui->edt_staffID->text().toInt();
    QString staffName = this->ui->edt_staffName->text();
    int staffSex = this->ui->comb_staffSex->currentIndex();
    int staffAge = this->ui->spb_staffAge->value();
    //取职位号
    int curRow=this->ui->comb_staffPosition->currentIndex();
    QModelIndex index = this->ui->comb_staffPosition->model()->index(curRow,0);
    QStandardItemModel* model = static_cast<QStandardItemModel*>(ui->comb_staffPosition->model());//拿到combobox里加入的model
    QStandardItem *item = model->itemFromIndex(index);//从model拿到item
    int positionID = item->accessibleText().toInt();
    //取科室号
    curRow=this->ui->comb_staffDepartment->currentIndex();
    index = this->ui->comb_staffDepartment->model()->index(curRow,0);
    model = static_cast<QStandardItemModel*>(ui->comb_staffDepartment->model());//拿到combobox里加入的model
    item = model->itemFromIndex(index);//从model拿到item
    int departmentID = item->toolTip().toInt();

    QString staffPassword = this->ui->edt_staffPassword->text();

    dbManager db;
    if(db.openDB()){

        QString sql = "update staff set staff_name=:staffName,staff_sex=:staffSex,staff_age=:staffAge,position_id=:positionID,department_id=:departmentID,staff_password=:staffPassword where staff_id=:staffID;";
        QSqlQuery query;
        query.prepare(sql);
        query.bindValue(":staffName",staffName);
        query.bindValue(":staffSex",staffSex);
        query.bindValue(":staffAge",staffAge);
        query.bindValue(":positionID",positionID);
        query.bindValue(":departmentID",departmentID);
        query.bindValue(":staffPassword",staffPassword);
        query.bindValue(":staffID",staffID);
        if(query.exec()){
            if(query.numRowsAffected()==0){
                qDebug()<<"数据失败";
            }
            else{qDebug()<<"更新成功";}
        }
        else{qDebug()<<query.lastError()<<"执行更新失败";}
    }
    else{qDebug()<<"数据库未开启";}
    initStaff();
}

void manageWindow::on_btn_delStaff_clicked()
{
    if(ui->edt_staffID->text().trimmed().isEmpty())
    {
        QMessageBox::information(NULL,"错误","请选中员工信息！",QMessageBox::Yes|QMessageBox::No,QMessageBox::Yes);
        return;
    }
    int staffID= this->ui->edt_staffID->text().toInt();

    dbManager db;
    if(db.openDB()){
        QString sql = "delete from staff where staff_id=:staffID;";
        QSqlQuery query;
        query.prepare(sql);
        query.bindValue(":staffID",staffID);
        if(query.exec()){
            if(query.numRowsAffected()==0){
                qDebug()<<"数据失败";
            }
            else{qDebug()<<"删除成功";}
        }
        else{qDebug()<<query.lastError()<<"执行删除失败";}
    }
    else{qDebug()<<"数据库未开启";}
    initStaff();
}


void manageWindow::initDuty()
{
    QStandardItemModel* pModel = new QStandardItemModel(ui->trv_duty);//取父节点
    pModel->setHorizontalHeaderLabels(QStringList()<<"科室号"<<"科室名"<<"值班序号"<<"员工序号"<<"员工姓名"<<"开始时间"<<"结束时间");//设置标题

    dbManager db;
    if(db.openDB()){
        QSqlQuery qDpm;
        if(qDpm.exec("select * from department")){
            while(qDpm.next()){
                QString dpm = qDpm.value("department_name").toString();
                QString dpm_id = qDpm.value("department_id").toString();
                QStandardItem * pItem = new QStandardItem(dpm_id);//给model赋第一列值
                pModel->appendRow(pItem);
                pModel->setItem(pModel->indexFromItem(pItem).row(),1, new QStandardItem(dpm));

                QSqlQuery qDuty;
                QString sql = "select * from duty,staff,department,dpmdetail where duty.staff_id=staff.staff_id and duty.dpmdetail_id=dpmdetail.dpmdetail_id and dpmdetail.department_id=department.department_id and dpmdetail.department_id="+dpm_id;

                if(qDuty.exec(sql)){
                    while(qDuty.next()){
                        QString dpmDtlName = qDuty.value("dpmdetail.dpmdetail_name").toString();
                        QString dpmDtlID = qDuty.value("dpmdetail.dpmdetail_id").toString();
                        QString dutyID = qDuty.value("duty.duty_id").toString();
                        QString staffID = qDuty.value("staff.staff_id").toString();
                        QString staffName = qDuty.value("staff.staff_name").toString();
                        QString dutyStart = qDuty.value("duty.start_date").toString();
                        QString dutyEnd = qDuty.value("duty.end_date").toString();
                        QStandardItem * cItem = new QStandardItem(dpmDtlID);//给model赋第一列值
                        pItem->appendRow(cItem);
                        pItem->setChild(cItem->index().row(),1, new QStandardItem(dpmDtlName));
                        pItem->setChild(cItem->index().row(),2, new QStandardItem(dutyID));//给model赋第二列值
                        pItem->setChild(cItem->index().row(),3, new QStandardItem(staffID));//给model赋第二列值
                        pItem->setChild(cItem->index().row(),4, new QStandardItem(staffName));
                        pItem->setChild(cItem->index().row(),5, new QStandardItem(dutyStart));
                        pItem->setChild(cItem->index().row(),6, new QStandardItem(dutyEnd));
                    }
                }
                else{qDebug()<<"详情数据查询出错"<<qDuty.lastError();}
            }
            ui->trv_duty->setModel(pModel);
            ui->trv_duty->expandAll();
            ui->trv_duty->setSortingEnabled(true);
        }
        else{qDebug()<<"数据查询出错";}
    }
    else{qDebug()<<"数据库未连接";}
}

//填充值班表编号
void manageWindow::on_trv_duty_clicked(const QModelIndex &index)
{
    this->ui->edt_dutyID->setText(index.sibling(index.row(),2).data().toString());
}

void manageWindow::on_btn_addDuty_clicked()
{
    EditDuty editduty(this);
    editduty.setPage(0);
    editduty.exec();
    initDuty();
}

void manageWindow::on_btn_updateDuty_clicked()
{
    if(this->ui->edt_dutyID->text().trimmed().isEmpty())
    {
        QMessageBox::information(NULL,"错误","没有选中值班信息！",QMessageBox::Yes|QMessageBox::No,QMessageBox::Yes);//错误处理
        return;
    }
    QString dutyID = this->ui->edt_dutyID->text();
    EditDuty editduty(this);
    editduty.setPage(1);
    editduty.initDuty(dutyID);
    editduty.exec();
    initDuty();
}

void manageWindow::on_btn_delDuty_clicked()
{
    if(this->ui->edt_dutyID->text().trimmed().isEmpty())
    {
        QMessageBox::information(NULL,"错误","没有选中值班信息！",QMessageBox::Yes|QMessageBox::No,QMessageBox::Yes);//错误处理
        return;
    }
    QString dutyID = this->ui->edt_dutyID->text();
    EditDuty editduty(this);
    editduty.setPage(2);
    editduty.initDuty(dutyID);
    editduty.exec();
    initDuty();
}

void manageWindow::on_tabWidget_2_currentChanged(int index)
{
    switch(index)
    {
    case 0:
        this->ui->stackedWidget_3->setCurrentIndex(0);
        initDpmStatistics();
        break;
    case 1:
        initDutyRecord();
        break;
    default:break;
    }
}

void manageWindow::initDpmStatistics()
{
    QStandardItemModel* pModel = new QStandardItemModel(ui->trv_dpmStatistics);//取父节点
    pModel->setHorizontalHeaderLabels(QStringList()<<"科室编号"<<"科室名"<<"处方编号"<<"开方日期"<<"医生名"<<"员工编号");//设置标题

    dbManager db;
    if(db.openDB()){
        QSqlQuery qDpm;
        if(qDpm.exec("select * from department;")){
            while(qDpm.next()){
                QString dpm_id = qDpm.value("department_id").toString();
                QString dpm = qDpm.value("department_name").toString();
                QStandardItem * pItem = new QStandardItem(dpm_id);//给model赋第一列值
                pModel->appendRow(pItem);
                pModel->setItem(pModel->indexFromItem(pItem).row(),1, new QStandardItem(dpm));//给model赋第二列值

                QSqlQuery qPsp;
                if(qPsp.exec("select * from psp,staff,department where psp.doctor_id=staff.staff_id and staff.department_id=department.department_id and staff.department_id="+dpm_id+";")){
                    while(qPsp.next()){
                        QString psp_id = qPsp.value("psp.psp_id").toString();
                        QString psp_date = qPsp.value("psp.psp_date").toString();
                        QString staff_id = qPsp.value("staff.staff_id").toString();
                        QString staff_name = qPsp.value("staff.staff_name").toString();
                        QStandardItem * cItem = new QStandardItem();//给model赋第一列值
                        pItem->appendRow(cItem);
                        pItem->setChild(cItem->index().row(),2, new QStandardItem(psp_id));//给model赋第二列值
                        pItem->setChild(cItem->index().row(),3, new QStandardItem(psp_date));
                        pItem->setChild(cItem->index().row(),4, new QStandardItem(staff_name));
                        pItem->setChild(cItem->index().row(),5, new QStandardItem(staff_id));
                    }
                }
                else{qDebug()<<"详情数据查询出错";}
            }
            ui->trv_dpmStatistics->setModel(pModel);
            ui->trv_dpmStatistics->expandAll();
            ui->trv_dpmStatistics->setSortingEnabled(true);
            for(int i = 0;i < 5 ;i++)//宽度自适应
            {
                ui->trv_dpmStatistics->resizeColumnToContents(i);

            }
        }
        else{qDebug()<<"数据查询出错";}
    }
    else{qDebug()<<"数据库未连接";}
}

void manageWindow::initDilyStatistics()
{
    //取当前时间
    QDate curDt=QDate::currentDate();

    QStandardItemModel* pModel = new QStandardItemModel(ui->trv_dailyStatistics);//取父节点
    pModel->setHorizontalHeaderLabels(QStringList()<<"科室编号"<<"科室名"<<"处方编号"<<"开方日期"<<"医生名"<<"员工编号");//设置标题

    dbManager db;
    if(db.openDB()){
        QSqlQuery qDpm;
        if(qDpm.exec("select * from department;")){
            while(qDpm.next()){
                QString dpm_id = qDpm.value("department_id").toString();
                QString dpm = qDpm.value("department_name").toString();
                QStandardItem * pItem = new QStandardItem(dpm_id);//给model赋第一列值
                pModel->appendRow(pItem);
                pModel->setItem(pModel->indexFromItem(pItem).row(),1, new QStandardItem(dpm));//给model赋第二列值

                QSqlQuery qPsp;
                if(qPsp.exec("select * from psp,staff,department where psp.doctor_id=staff.staff_id and staff.department_id=department.department_id and staff.department_id="+dpm_id+";")){
                    while(qPsp.next()){
                        QDate pspDt = qPsp.value("psp.psp_date").toDate();
                        if(curDt!=pspDt){continue;}

                        QString psp_date = qPsp.value("psp.psp_date").toString();
                        QString psp_id = qPsp.value("psp.psp_id").toString();
                        QString staff_id = qPsp.value("staff.staff_id").toString();
                        QString staff_name = qPsp.value("staff.staff_name").toString();
                        QStandardItem * cItem = new QStandardItem();//给model赋第一列值
                        pItem->appendRow(cItem);
                        pItem->setChild(cItem->index().row(),2, new QStandardItem(psp_id));//给model赋第二列值
                        pItem->setChild(cItem->index().row(),3, new QStandardItem(psp_date));
                        pItem->setChild(cItem->index().row(),4, new QStandardItem(staff_name));
                        pItem->setChild(cItem->index().row(),5, new QStandardItem(staff_id));
                    }
                }
                else{qDebug()<<"详情数据查询出错";}
            }
            ui->trv_dailyStatistics->setModel(pModel);
            ui->trv_dailyStatistics->expandAll();
            ui->trv_dailyStatistics->setSortingEnabled(true);
            for(int i = 0;i < 5 ;i++)//宽度自适应
            {
                ui->trv_dailyStatistics->resizeColumnToContents(i);

            }
        }
        else{qDebug()<<"数据查询出错";}
    }
    else{qDebug()<<"数据库未连接";}
}

void manageWindow::on_btn_dpmStatistics_clicked()
{
    this->ui->stackedWidget_3->setCurrentIndex(0);
    initDpmStatistics();
}

void manageWindow::on_btn_dailyStatistics_clicked()
{
    this->ui->stackedWidget_3->setCurrentIndex(1);
    initDilyStatistics();
}



void manageWindow::on_btn_charts_clicked()
{
    ShowChart showChart(this);
    if(this->ui->stackedWidget_3->currentIndex()==0){
        //科室统计图表
        showChart.showDpmCharts();
    }
    else if(this->ui->stackedWidget_3->currentIndex()==1){
        //每日统计图表
        QDate curDt=QDate::currentDate();
        showChart.showDailyCharts(curDt);
    }
    showChart.exec();
}

void manageWindow::initDutyRecord()
{
    QStandardItemModel* pModel = new QStandardItemModel(ui->trv_dutyRecord);//取父节点
    pModel->setHorizontalHeaderLabels(QStringList()<<"科室编号"<<"科室名"<<"登陆编号"<<"员工编号"<<"员工名"<<"登陆日期"<<"注销日期"<<"值班状态");//设置标题

    dbManager db;
    if(db.openDB()){
        QSqlQuery qDpm;
        if(qDpm.exec("select * from department;")){
            while(qDpm.next()){
                QString dpm_id = qDpm.value("department_id").toString();
                QString dpm = qDpm.value("department_name").toString();
                QStandardItem * pItem = new QStandardItem(dpm_id);//给model赋第一列值
                pModel->appendRow(pItem);
                pModel->setItem(pModel->indexFromItem(pItem).row(),1, new QStandardItem(dpm));//给model赋第二列值

                QSqlQuery qDutyRecord;
                if(qDutyRecord.exec("select * from dutyrecord,staff,department where dutyrecord.staff_id=staff.staff_id and staff.department_id=department.department_id and staff.department_id="+dpm_id+";")){
                    while(qDutyRecord.next()){
                        QString dutyrecord_id = qDutyRecord.value("dutyrecord.dutyrecord_id").toString();
                        QString staff_id = qDutyRecord.value("staff.staff_id").toString();
                        QString staff_name = qDutyRecord.value("staff.staff_name").toString();
                        QString dutyrecord_start_date = qDutyRecord.value("dutyrecord.real_start_date").toString();
                        QString dutyrecord_end_date = qDutyRecord.value("dutyrecord.real_end_date").toString();
                        QString dutyrecord_status = qDutyRecord.value("dutyrecord.dutyrecord_status").toString();
                        QStandardItem * cItem = new QStandardItem();//给model赋第一列值
                        pItem->appendRow(cItem);
                        pItem->setChild(cItem->index().row(),2, new QStandardItem(dutyrecord_id));//给model赋第二列值
                        pItem->setChild(cItem->index().row(),3, new QStandardItem(staff_name));
                        pItem->setChild(cItem->index().row(),4, new QStandardItem(staff_id));
                        pItem->setChild(cItem->index().row(),5, new QStandardItem(dutyrecord_start_date));
                        pItem->setChild(cItem->index().row(),6, new QStandardItem(dutyrecord_end_date));
                        pItem->setChild(cItem->index().row(),7, new QStandardItem(dutyrecord_status));
                    }
                }
                else{qDebug()<<"详情数据查询出错";}
            }
            ui->trv_dutyRecord->setModel(pModel);
            ui->trv_dutyRecord->expandAll();
            ui->trv_dutyRecord->setSortingEnabled(true);
            for(int i = 0;i < 8 ;i++)//宽度自适应
            {
                ui->trv_dutyRecord->resizeColumnToContents(i);

            }
        }
        else{qDebug()<<"数据查询出错";}
    }
    else{qDebug()<<"数据库未连接";}
}


