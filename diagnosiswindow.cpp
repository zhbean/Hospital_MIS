#include "diagnosiswindow.h"
#include "ui_diagnosiswindow.h"

diagnosisWindow::diagnosisWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::diagnosisWindow)
{
    ui->setupUi(this);
}

diagnosisWindow::~diagnosisWindow()
{
    delete ui;
}
