# -------------------------------------------------
# Project created by QtCreator 2009-09-20T17:20:55
# -------------------------------------------------
TARGET = znotes
VERSION = "0.4.4"
QT += core \
    gui
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
    note_text.cpp \
    note_html.cpp \
    note_picture.cpp \
    application.cpp
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
    note_text.h \
    note_html.h \
    note_picture.h \
    application.h
FORMS += mainwindow.ui \
    configdialog.ui \
    aboutDialog.ui
TRANSLATIONS += translations/znotes_ru.ts \
	translations/znotes_cs.ts \
	translations/znotes_pt_BR.ts \
	translations/znotes_uk.ts
RESOURCES += znotes.qrc
!without_single_inst {
    QT += network
    DEFINES += SINGLE_INSTANCE
    SOURCES += single_inst/qtlockedfile_win.cpp \
        single_inst/qtlockedfile_unix.cpp \
        single_inst/qtlockedfile.cpp \
        single_inst/qtlocalpeer.cpp
    HEADERS += single_inst/qtlockedfile.h \
        single_inst/qtlocalpeer.h
}
!without_todo_format {
    QT += xml
    DEFINES += NOTE_TODO_FORMAT
    SOURCES += note_todo.cpp \
		todomodel.cpp
    HEADERS += note_todo.h \
		todomodel.h
}

# This makes qmake generate translations
isEmpty(QMAKE_LRELEASE):QMAKE_LRELEASE = $$[QT_INSTALL_BINS]/lrelease
TSQM.name = $$QMAKE_LRELEASE \
    ${QMAKE_FILE_IN}
TSQM.input = TRANSLATIONS
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
