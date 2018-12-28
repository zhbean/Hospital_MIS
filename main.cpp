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
    c.exec();
    int b=0;
    manageWindow m;
    diagnosisWindow d;
    registerWindow r;
    PharmacyDialog p;

    switch(b) {
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
    }

    return a.exec();
}
