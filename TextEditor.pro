TARGET = txtedit
TEMPLATE = app

QT += core gui
greaterThan(QT_MAJOR_VERSION, 4) QT += widgets

DEFINES += \
    GIT_TAG=\\\"$$system(git --git-dir $$PWD/.git --work-tree $$PWD describe --always --tags)\\\"

SOURCES += main.cpp \
    mainwindow.cpp \
    textedit.cpp

HEADERS += \
    mainwindow.h \
    textedit.h

RESOURCES += \
    icons.qrc
