#include "zapplication.h"
#include "mainwindow.h"
#include "settings.h"

int main(int argc, char **argv)
{
	zApplication app(argc, argv);

	settings.load();
    // if another copy is starts
    if(app.isRunning() && settings.getSingleInstance()) {
        return !app.sendMessage("Only one instance is allowed");
    }

	app.setQuitOnLastWindowClosed(false);

    MainWindow w;
	app.setActivationWindow(&w);

    return app.exec();
}
