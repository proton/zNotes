#include <QtGui/QApplication>
#include "mainwindow.h"
//#include "settings.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
	w.show();
	//if(Settings::getHideStart()) w.hide();
    return a.exec();
}
