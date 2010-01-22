#include <QtGui/QApplication>
#include "mainwindow.h"
#include <QtDebug>

class zApplication: public QApplication
{
public:
	zApplication(int &argc, char **argv) : QApplication(argc, argv) {}
	~zApplication() {}
	void commitData(QSessionManager &) {} //fixing "KDE kogout was cancelled by zNotes" bug
};

int main(int argc, char *argv[])
{
	zApplication a(argc, argv);
	a.setQuitOnLastWindowClosed(false);
	MainWindow w;
	return a.exec();
}
