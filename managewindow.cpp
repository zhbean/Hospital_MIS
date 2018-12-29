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

}

void manageWindow::showSetting()
{
    ui->stackedWidget->setCurrentIndex(0);
}

void manageWindow::showStatistics()
{
    ui->stackedWidget->setCurrentIndex(1);
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
        else{qDebug()<<query.lastError()<<"执行删除失败";}
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
        else{qDebug()<<query.lastError()<<"执行删除失败";}
    }
    else{qDebug()<<"数据库未开启";}
}
