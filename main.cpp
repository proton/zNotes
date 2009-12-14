#include <QtGui/QApplication>
#include <QLocale>
#include <QTranslator>
#include <QLibraryInfo>
#include "mainwindow.h"

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	a.setQuitOnLastWindowClosed(false);

	//Loading translation
	QTranslator translator, qtranslator;
	QString locale = QLocale::system().name();
	qtranslator.load("qt_"+locale, QLibraryInfo::location(QLibraryInfo::TranslationsPath));
	a.installTranslator( &qtranslator );
#ifdef PROGRAM_DATA_DIR
	if(!translator.load(QString(PROGRAM_DATA_DIR) + "/translations/znotes_" + locale ) )
#endif
	if(!translator.load( QCoreApplication::applicationDirPath() + "/translations/znotes_" + locale ))
	translator.load( QCoreApplication::applicationDirPath() + "/znotes_" + locale );
	a.installTranslator(&translator);
	//
	MainWindow w;
	return a.exec();
}
