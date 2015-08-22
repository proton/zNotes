# -------------------------------------------------
# Project created by QtCreator 2009-09-20T17:20:55
# -------------------------------------------------
TARGET = znotes
VERSION = "0.4.5"

QT += core network
if(greaterThan(QT_MAJOR_VERSION, 4)) {
   QT += widgets printsupport
} else {
   QT += gui
}
TEMPLATE = app

OBJECTS_DIR = build
UI_DIR = build
MOC_DIR = build
RCC_DIR = build

SOURCES += main.cpp \
	mainwindow.cpp \
	configdialog.cpp \
	settings.cpp \
	note.cpp \
	scriptmodel.cpp \
	aboutDialog.cpp \
	toolbarmodel.cpp \
	highlighter.cpp \
	textedit.cpp \
	notelist.cpp \
	notecreatewidget.cpp \
	notetype.cpp \
	notecreatebutton.cpp \
	highlightrulemodel.cpp \
	single_inst/qtlockedfile_win.cpp \
	single_inst/qtlockedfile_unix.cpp \
	single_inst/qtlockedfile.cpp \
	single_inst/qtlocalpeer.cpp \
	single_inst/qtsingleapplication.cpp \
    ztabwidget.cpp \
    ztabbar.cpp \
    shared.cpp \
    todonote.cpp \
    textnote.cpp \
    picturenote.cpp \
    htmlnote.cpp \
    zapplication.cpp

HEADERS += mainwindow.h \
	configdialog.h \
	settings.h \
	note.h \
	scriptmodel.h \
	aboutDialog.h \
	toolbaraction.h \
	toolbarmodel.h \
	highlighter.h \
	textedit.h \
	notelist.h \
	notecreatewidget.h \
	notetype.h \
	notecreatebutton.h \
	highlightrulemodel.h \
	single_inst/qtlockedfile.h \
	single_inst/qtlocalpeer.h \
	single_inst/qtsingleapplication.h \
    ztabwidget.h \
    ztabbar.h \
    shared.h \
    textnote.h \
    htmlnote.h \
    picturenote.h \
    todonote.h \
    zapplication.h

FORMS += mainwindow.ui \
	configdialog.ui \
	aboutDialog.ui \
	notecreatewidget.ui

TRANSLATIONS += translations/znotes_ru.ts \
                translations/znotes_cs.ts \
                translations/znotes_pl.ts \
                translations/znotes_pt_BR.ts \
                translations/znotes_uk.ts \
                translations/znotes_sk.ts \
                translations/znotes_es.ts \
                translations/znotes_de.ts \
                translations/znotes_en.ts \
                translations/znotes_sv.ts
# For Qt translation files only qm-files compiling. No lupdate.
QT_TRANSLATIONS += translations/qt_ru.ts \
                   translations/qt_cs.ts \
                   translations/qt_pl.ts \
                   translations/qt_pt.ts \
                   translations/qt_uk.ts \
                   translations/qt_sk.ts \
                   translations/qt_es.ts \
                   translations/qt_de.ts \
                   translations/qt_sv.ts

RESOURCES += znotes.qrc

!without_todo_format {
	QT += xml
	DEFINES += NOTE_TODO_FORMAT
	SOURCES += \
		todomodel.cpp
	HEADERS += \
		todomodel.h
}

# This makes qmake generate translations
isEmpty(QMAKE_LRELEASE):QMAKE_LRELEASE = $$[QT_INSTALL_BINS]/lrelease
TSQM.name = $$QMAKE_LRELEASE \
    ${QMAKE_FILE_IN}
TSQM.input = TRANSLATIONS QT_TRANSLATIONS
TSQM.output = ${QMAKE_FILE_BASE}.qm
TSQM.commands = $$QMAKE_LRELEASE \
    ${QMAKE_FILE_IN}
TSQM.CONFIG = no_link
QMAKE_EXTRA_COMPILERS += TSQM
PRE_TARGETDEPS += compiler_TSQM_make_all
!os2:DEFINES += VERSION=\\\"$$VERSION\\\"

unix {
    PREFIX = $$(PREFIX)
    isEmpty( PREFIX ):PREFIX = /usr
    DEFINES += PROGRAM_DATA_DIR=\\\"$$PREFIX/share/znotes/\\\"
    target.path = $$PREFIX/bin/
    locale.path = $$PREFIX/share/znotes/translations/
    locale.files = translations/*.qm
    pixmap.path = /usr/share/pixmaps
    pixmap.files = *.png
    desktop.path = /usr/share/applications
    desktop.files = *.desktop
    INSTALLS += target \
        locale \
        pixmap \
        desktop
}

os2 {
    DEFINES += VERSION=\'\"$$VERSION\"\'
    RC_FILE = znotes_os2.rc
}

win32:RC_FILE = znotes.rc
