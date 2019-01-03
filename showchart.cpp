#include "showchart.h"
#include "ui_showchart.h"

ShowChart::ShowChart(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ShowChart)
{
    ui->setupUi(this);
}

ShowChart::~ShowChart()
{
    delete ui;
}

void ShowChart::showDpmCharts()
{
    QChart *chart = new QChart();
    QDateTimeAxis dateAxisX;
    QLineSeries* line;
    QScatterSeries *scatter;

    dbManager db;
    if(db.getDB()){
        QSqlQuery qDpm;
        if(qDpm.exec("select * from department;")){
            while(qDpm.next()){
                QDate curDt = QDate::currentDate().addDays(-30);
                line = new QLineSeries();
                scatter = new QScatterSeries();
                scatter->setMarkerSize(12); //离散点大小
                QString dpm = qDpm.value("department_name").toString();
                QString dpm_id = qDpm.value("department_id").toString();
                line->setName(dpm);
                for(int i=0;i<=30;i++){
                    QSqlQuery qPsp;
                    int dayPspCount =0;
                    if(qPsp.exec("select * from psp,staff,department where psp.doctor_id=staff.staff_id and staff.department_id=department.department_id and staff.department_id="+dpm_id+";")){
                        while(qPsp.next()){
                            QDate pspDt = qPsp.value("psp.psp_date").toDate();
                            if(curDt==pspDt){dayPspCount++;}
                        }
                    }
                    else{qDebug()<<"详情数据查询出错";}
                    QDateTime iDt;
                    iDt.setDate(curDt);
                    line->append(iDt.toMSecsSinceEpoch(),dayPspCount);
                    scatter->append(iDt.toMSecsSinceEpoch(),dayPspCount);
                    curDt=curDt.addDays(1);
                }
                chart->addSeries(line);
                chart->addSeries(scatter);
                dateAxisX.setFormat("yy-MM-dd");//设置X轴数据的显示格式
                chart->setAxisX(&dateAxisX, line);
                connect(scatter, &QScatterSeries::hovered, this, &ShowChart::slotPointHoverd);//用于鼠标移动到点上显示数值
            }
        }
        else{qDebug()<<"数据查询出错";}
    }
    else{qDebug()<<"数据库未连接";}


    chart->createDefaultAxes();
    chart->setTitle("30天变化曲线");
    chart->setTheme(QChart::ChartThemeQt);
    QChartView *chartview = new QChartView(this);
    chartview->setChart(chart);
    chartview->show();

    ui->verticalLayout->insertWidget(0, chartview);

}

//槽鼠标放置限制
void ShowChart::slotPointHoverd(const QPointF &point, bool state)
{
    if (state){
        ui->m_valueLabel->setText(QString::asprintf("%1.0f", point.y()));
        QPoint curPos = mapFromGlobal(QCursor::pos());
        ui->m_valueLabel->move(curPos.x() - ui->m_valueLabel->width() / 2, curPos.y() - ui->m_valueLabel->height() * 1.5);//移动数值
        ui->m_valueLabel->show();//显示出来
    }
    //else ui->m_valueLabel->hide();//进行隐藏
    ui->verticalLayout->insertWidget(1, ui->m_valueLabel);
}


void ShowChart::showDailyCharts(QDate curDt)
{
    QBarSeries *series = new QBarSeries();//柱状图
    dbManager db;
    if(db.getDB()){
        QSqlQuery qDpm;
        if(qDpm.exec("select * from department;")){
            while(qDpm.next()){
                QString dpm = qDpm.value("department_name").toString();
                QString dpm_id = qDpm.value("department_id").toString();
                QBarSet *set = new QBarSet(dpm);//有几个科室一个柱子
                int pspCount = 0;//科室看病数

                QSqlQuery qPsp;
                if(qPsp.exec("select * from psp,staff,department where psp.doctor_id=staff.staff_id and staff.department_id=department.department_id and staff.department_id="+dpm_id+";")){
                    while(qPsp.next()){
                        QDate pspDt = qPsp.value("psp.psp_date").toDate();
                        if(curDt!=pspDt){continue;}
                        pspCount++;
                    }
                }
                else{qDebug()<<"详情数据查询出错";}
                *set<<pspCount;
                series->append(set);
            }
        }
        else{qDebug()<<"数据查询出错";}
    }
    else{qDebug()<<"数据库未连接";}

    QChart *chart = new QChart();
    chart->addSeries(series);
    chart->createDefaultAxes();
    chart->legend()->setAlignment(Qt::AlignRight);

    chart->setTitle("每日开药统计");
    chart->setTheme(QChart::ChartThemeQt);
    QChartView *chartview = new QChartView(this);
    chartview->setChart(chart);
    chartview->show();

    ui->verticalLayout->insertWidget(0, chartview);
}



