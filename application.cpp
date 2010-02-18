#include "application.h"
#include "settings.h"

#include <QWidget>

zApplication::zApplication(int &argc, char **argv)
	: QApplication(argc, argv), main_window(NULL)
{
	//Making something (locking file), etc.
}

bool zApplication::hasRanCopy()
{
	//Checking for something
	return false;
}

void zApplication::copyStarted()
{
	if(main_window!=NULL) main_window->show();
}

void zApplication::commitData(QSessionManager &) //fixing "KDE kogout was cancelled by zNotes" bug
{
	settings.save();
}
