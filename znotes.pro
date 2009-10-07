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
TRANSLATIONS += translations/znotes_ru.ts
RESOURCES += znotes.qrc

# This makes qmake generate translations
isEmpty(QMAKE_LRELEASE):QMAKE_LRELEASE = $$[QT_INSTALL_BINS]/lrelease
TS_OUT = $$TRANSLATIONS
TS_OUT ~= s/.ts/.qm
TSQM.name = lrelease \
	${QMAKE_FILE_IN}
TSQM.input = TRANSLATIONS
TSQM.output = $$TS_OUT
TSQM.commands = $$QMAKE_LRELEASE \
	${QMAKE_FILE_IN}
TSQM.CONFIG = no_link
QMAKE_EXTRA_COMPILERS += TSQM
PRE_TARGETDEPS += $$TS_OUT
