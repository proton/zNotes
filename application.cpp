#include "application.h"
#include "settings.h"

#ifdef SINGLE_INSTANCE
#include "single_inst/qtlocalpeer.h"
#endif

#include <QWidget>

void zApplication::_init(const QString &appId)
{
#ifdef SINGLE_INSTANCE
	peer = new QtLocalPeer(this, appId);
	connect(peer, SIGNAL(messageReceived(const QString&)), SLOT(copyStarted(const QString&)));
#else
	Q_UNUSED(appId)
#endif
}

zApplication::zApplication(int &argc, char **argv)
	: QApplication(argc, argv), main_window(NULL)
{
	_init("zNotes");
}

bool zApplication::sendMessage(const QString &message, int timeout)
{
	return peer->sendMessage(message, timeout);
}

void zApplication::copyStarted(const QString&)
{
	//Raising main window on copy start
	if(main_window!=NULL && settings.getCopyStartRaise())
	{
		main_window->show();
	}
}

void zApplication::commitData(QSessionManager &) //fixing "KDE kogout was cancelled by zNotes" bug
{
	settings.save();
}
