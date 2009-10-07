# -------------------------------------------------
# Project created by QtCreator 2009-09-20T17:20:55
# -------------------------------------------------
TARGET = znotes
QT += core \
    gui
TEMPLATE = app
SOURCES += main.cpp \
    mainwindow.cpp \
    configdialog.cpp \
    settings.cpp \
    note.cpp
HEADERS += mainwindow.h \
    configdialog.h \
    settings.h \
    note.h
FORMS += mainwindow.ui \
    configdialog.ui
TRANSLATIONS = translations/znotes_ru.ts
RESOURCES += znotes.qrc
