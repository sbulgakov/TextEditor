TARGET = txtedit
TEMPLATE = app

QT += core gui
greaterThan(QT_MAJOR_VERSION, 4) QT += widgets

DEFINES += \
    GIT_TAG=\\\"$$system(git --git-dir $$PWD/.git --work-tree $$PWD describe --always --tags)\\\" \
    GIT_DATE=\\\"$$system(git --git-dir $$PWD/.git --work-tree $$PWD log -n1 --format=format:%as)\\\" \
    GIT_HASH=\\\"$$system(git --git-dir $$PWD/.git --work-tree $$PWD log -n1 --format=format:%h)\\\" \
    GIT_ABBR=\\\"$$system(git --git-dir $$PWD/.git --work-tree $$PWD describe --always --tags --abbrev=0)\\\"

DEFINES += \
    FINDDIALOG_ALL \
    FINDDIALOG_REPLACE \
    FINDDIALOG_RESULTS

CONFIG += hunspell

DEFINES += \
    TEXTEDIT_MENU

DEFINES += \
    HAVE_SETTINGS

SOURCES += main.cpp \
    settings.cpp \
    mainwindow.cpp \
    finddialog.cpp \
    highlighter.cpp \
    textedit.cpp

HEADERS += \
    settings.h \
    mainwindow.h \
    finddialog.h \
    highlighter.h \
    textedit.h

RESOURCES += \
    icons.qrc

icons {
    DEFINES += HAVE_ICONS
    RESOURCES += \
        icons-file.qrc \
        icons-edit.qrc \
        icons-find.qrc \
        icons-help.qrc
}

win32:hunspell {
    DEPENDPATH += ../TextEditor/hunspell
    INCLUDEPATH += ../TextEditor/hunspell/src/
    LIBS += -L../TextEditor/hunspell/lib
}
hunspell {
    DEFINES += HAVE_HUNSPELL
    LIBS += -lhunspell
    SOURCES += hunspellhighlighter.cpp
    HEADERS += hunspellhighlighter.h
}
