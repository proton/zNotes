#include "zapplication.h"
#include "settings.h"

zApplication::zApplication(int& argc, char** argv, bool GUIenabled)
	: QtSingleApplication(argc, argv, GUIenabled) {}
zApplication::zApplication(const QString& id, int& argc, char** argv)
	: QtSingleApplication(id, argc, argv) {}
zApplication::zApplication(int& argc, char** argv, Type type)
	: QtSingleApplication(argc, argv, type) {}

#if defined(Q_WS_X11)
zApplication::zApplication(Display* dpy, Qt::HANDLE visual, Qt::HANDLE colormap)
	: QtSingleApplication(dpy, visual, colormap) {}
zApplication::zApplication(Display* dpy, int& argc, char** argv, Qt::HANDLE visual, Qt::HANDLE cmap)
	: QtSingleApplication(dpy, argc, argv, visual, cmap) {}
zApplication::zApplication(Display* dpy, const QString& appId, int argc, char** argv, Qt::HANDLE visual, Qt::HANDLE colormap)
	: QtSingleApplication(dpy, appId, argc, argv, visual, colormap) {}
#endif

void zApplication::commitData(QSessionManager &) //fixing "KDE logout was cancelled by zNotes" bug
{
	settings.save();
}
