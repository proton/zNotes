#include "application.h"
#include "mainwindow.h"
#include "settings.h"

int main(int argc, char *argv[])
{
	zApplication a(argc, argv);
	settings.load();
	//if another copy is startes
	if(a.sendMessage("proton is our god!") /*&& settings.*/)
		return 0;
	a.setQuitOnLastWindowClosed(false);
	MainWindow w;
	a.setMainWindow(&w);
	return a.exec();
}
