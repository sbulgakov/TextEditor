TARGET = txtedit
TEMPLATE = app

QT += core gui
greaterThan(QT_MAJOR_VERSION, 4) QT += widgets

SOURCES += main.cpp \
    mainwindow.cpp \
    textedit.cpp

HEADERS += \
    mainwindow.h \
    textedit.h

RESOURCES += \
    icons.qrc
