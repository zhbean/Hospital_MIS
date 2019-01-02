#ifndef SHOWCHART_H
#define SHOWCHART_H

#include <QDialog>
#include <QChart>
#include <QChartView>
#include <QLineSeries>
#include <QBarSet>
#include <QBarSeries>
#include <QtCharts/QPieSeries>
#include <QtCharts/QPieSlice>
#include "dbManager.h"
#include "QDate"
#include "QDateTimeAxis"
#include "QScatterSeries"
#include "QLabel"
using namespace QtCharts;//不加这句话无法使用

namespace Ui {
class ShowChart;
}

class ShowChart : public QDialog
{
    Q_OBJECT

public:
    explicit ShowChart(QWidget *parent = 0);
    ~ShowChart();
    void showDpmCharts();
    void showDailyCharts(QDate curDt);

private:
    Ui::ShowChart *ui;

private slots:
    void slotPointHoverd(const QPointF &point, bool state);

};

#endif // SHOWCHART_H
