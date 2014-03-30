#ifndef APPLICATION_H
#define APPLICATION_H

#include "single_inst/qtsingleapplication.h"

class zApplication: public QtSingleApplication
{
	Q_OBJECT
public:
	zApplication(int &argc, char **argv, bool GUIenabled = true);
    zApplication(const QString &id, int &argc, char **argv);
#if QT_VERSION < 0x050000
    QtSingleApplication(int &argc, char **argv, Type type);
    zApplication(int &argc, char **argv, Type type);
#if defined(Q_WS_X11)
	zApplication(Display* dpy, Qt::HANDLE visual = 0, Qt::HANDLE colormap = 0);
	zApplication(Display* dpy, int &argc, char **argv, Qt::HANDLE visual = 0, Qt::HANDLE cmap= 0);
	zApplication(Display* dpy, const QString &appId, int argc, char **argv, Qt::HANDLE visual = 0, Qt::HANDLE colormap = 0);
#endif
#endif
	~zApplication() {}
	void commitData(QSessionManager &); //fixing "KDE kogout was cancelled by zNotes" bug
};

#endif // APPLICATION_H
