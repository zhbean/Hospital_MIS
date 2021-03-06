#-------------------------------------------------
#
# Project created by QtCreator 2018-12-24T00:08:41
#
#-------------------------------------------------

QT       += core gui
QT       += printsupport
QT       += charts


greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Hosptial_MIS
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += \
        main.cpp \
        dbManager.cpp \
      login.cpp \
    pharmacydialog.cpp\
    diagnosiswindow.cpp \
    managewindow.cpp \
    registerwindow.cpp \
    patientdialog.cpp \
    medicinedialog.cpp \
    infodialog.cpp \
    editduty.cpp \
    historymedicinedialog.cpp \
    showchart.cpp \
    updatepricedialog.cpp

HEADERS += \
        dbManager.h \
      login.h \
    pharmacydialog.h\
    diagnosiswindow.h \
    managewindow.h \
    registerwindow.h \
    patientdialog.h \
    medicinedialog.h \
    infodialog.h \
    editduty.h \
    historymedicinedialog.h \
    showchart.h \
    updatepricedialog.h




FORMS += \
      login.ui \
    diagnosiswindow.ui \
    managewindow.ui \
   pharmacydialog.ui\
    registerwindow.ui \
    patientdialog.ui \
    medicinedialog.ui \
    infodialog.ui \
    editduty.ui \
    historymedicinedialog.ui \
    showchart.ui \
    updatepricedialog.ui


QT += sql

RESOURCES += \
    picture.qrc
