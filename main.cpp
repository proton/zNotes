#include "application.h"
#include "mainwindow.h"

int main(int argc, char *argv[])
{
	zApplication a(argc, argv);
	a.setQuitOnLastWindowClosed(false);
	MainWindow w;
	return a.exec();
}
