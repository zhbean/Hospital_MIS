#include "managewindow.h"
#include "ui_managewindow.h"

manageWindow::manageWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::manageWindow)
{
    ui->setupUi(this);
}

manageWindow::~manageWindow()
{
    delete ui;
}
