#ifndef ZAPPLICATION_H
#define ZAPPLICATION_H

#include "single_inst/qtsingleapplication.h"

class zApplication: public QtSingleApplication
{
	Q_OBJECT
public:
	zApplication(int &argc, char **argv, bool GUIenabled = true);
	zApplication(const QString &id, int &argc, char **argv);
	zApplication(int &argc, char **argv, Type type);
#if defined(Q_WS_X11)
	zApplication(Display* dpy, Qt::HANDLE visual = 0, Qt::HANDLE colormap = 0);
	zApplication(Display* dpy, int &argc, char **argv, Qt::HANDLE visual = 0, Qt::HANDLE cmap= 0);
	zApplication(Display* dpy, const QString &appId, int argc, char **argv, Qt::HANDLE visual = 0, Qt::HANDLE colormap = 0);
#endif
	~zApplication() {}
    void commitData(QSessionManager &); //fixing "KDE logout was cancelled by zNotes" bug
};

#endif // ZAPPLICATION_H
