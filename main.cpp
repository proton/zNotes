#include <QtGui/QApplication>
#include <QLocale>
#include <QTranslator>
#include <QLibraryInfo>
#include "mainwindow.h"

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	a.setQuitOnLastWindowClosed(false);
	//
	QTranslator translator;
	QString locale = QLocale::system().name();

#ifdef unix
	if(!translator.load( "/usr/share/znotes/translations/znotes_" + locale ) )
#endif
		translator.load( QCoreApplication::applicationDirPath() + "/translations/znotes_" + locale );
	a.installTranslator(&translator);
	//
	MainWindow w;
	return a.exec();
}
