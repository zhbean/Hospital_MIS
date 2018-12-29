#include <QApplication>
#include<QObject>
#include "dbManager.h"
#include "login.h"
#include "qdebug.h"
#include "managewindow.h"
#include "diagnosiswindow.h"
#include "registerwindow.h"
#include "pharmacydialog.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Login c;
    manageWindow m;
    diagnosisWindow d;
    registerWindow r;
    PharmacyDialog p;
    QObject::connect(&c,SIGNAL(setAccountAndToolTip(int,int)),&d,SLOT(getAccountAndToolTip(int,int)));
    QObject::connect(&c,SIGNAL(setAccountAndToolTip(int,int)),&m,SLOT(getAccountAndToolTip(int,int)));
    QObject::connect(&c,SIGNAL(setAccountAndToolTip(int,int)),&r,SLOT(getAccountAndToolTip(int,int)));
    QObject::connect(&c,SIGNAL(setAccountAndToolTip(int,int)),&p,SLOT(getAccountAndToolTip(int,int)));

    if(c.exec()==QDialog::Accepted){
        qDebug()<<c.limit;
        switch(c.limit){
        case 0:
            r.show();
            break;
        case 1:
            d.show();
            break;
        case 2:
            p.show();
            break;
        case 3:
            m.show();
            break;
        default: break;
        }

        return a.exec();
    }
    return 0;
}
