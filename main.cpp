#include <QtGui/QApplication>
#include "mainwindow.h"

class zApplication: public QApplication
{
public:
	zApplication(int &argc, char **argv) : QApplication(argc, argv) {}
	~zApplication() {}
	void commitData(QSessionManager &)//fixing "KDE kogout was cancelled by zNotes" bug
	{
		settings.save();
	}
};

int main(int argc, char *argv[])
{
	zApplication a(argc, argv);
	a.setQuitOnLastWindowClosed(false);
	MainWindow w;
	return a.exec();
}
