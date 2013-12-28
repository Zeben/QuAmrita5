#-------------------------------------------------
#
# Project created by QtCreator 2013-02-11T10:48:47
#
#-------------------------------------------------

QT       += core gui sql printsupport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = QuAmrita5
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    settings.cpp \
    addclient.cpp \
    editclient.cpp \
    catalog.cpp \
    catalogadd.cpp \
    catalogedit.cpp \
    catalogdiscount.cpp \
    overhead.cpp

HEADERS  += mainwindow.h \
    settings.h \
    addclient.h \
    editclient.h \
    catalog.h \
    catalogadd.h \
    catalogedit.h \
    catalogdiscount.h \
    overhead.h

FORMS    += mainwindow.ui \
    settings.ui \
    addclient.ui \
    editclient.ui \
    catalog.ui \
    catalogadd.ui \
    catalogedit.ui \
    catalogdiscount.ui \
    overhead.ui

RESOURCES += \
    media.qrc

RC_FILE = icon.rc
