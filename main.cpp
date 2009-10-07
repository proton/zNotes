#include <QtGui/QApplication>
#include <QLocale>
#include <QTranslator>
#include <QLibraryInfo>
#include "mainwindow.h"

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	a.setQuitOnLastWindowClosed(false);
	QString locale = QLocale::system().name();
	QTranslator translator;
	translator.load(QString("znotes_") + locale, QLibraryInfo::location(QLibraryInfo::TranslationsPath));
	a.installTranslator(&translator);
	MainWindow w;
	return a.exec();
}
