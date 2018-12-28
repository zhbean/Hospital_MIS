#include <QApplication>
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
    if(c.exec()==QDialog::Accepted){
        qDebug()<<c.limit;
        manageWindow m;
        diagnosisWindow d;
        registerWindow r;
        PharmacyDialog p;
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
