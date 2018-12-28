#include "registerwindow.h"
#include "ui_registerwindow.h"

registerWindow::registerWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::registerWindow)
{
    ui->setupUi(this);
}

registerWindow::~registerWindow()
{
    delete ui;
}
