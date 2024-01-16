TARGET = txtedit
TEMPLATE = app

QT += core gui
greaterThan(QT_MAJOR_VERSION, 4) QT += widgets

DEFINES += \
    GIT_TAG=\\\"$$system(git --git-dir $$PWD/.git --work-tree $$PWD describe --always --tags)\\\"

DEFINES += \
    FINDDIALOG_ALL \
    FINDDIALOG_REPLACE \
    FINDDIALOG_RESULTS

SOURCES += main.cpp \
    mainwindow.cpp \
    finddialog.cpp \
    textedit.cpp

HEADERS += \
    mainwindow.h \
    finddialog.h \
    textedit.h

RESOURCES += \
    icons.qrc
