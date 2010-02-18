#ifndef APPLICATION_H
#define APPLICATION_H

#include <QtGui/QApplication>

class zApplication: public QApplication
{
	Q_OBJECT
public:
	zApplication(int &argc, char **argv);
	~zApplication() {}
	void commitData(QSessionManager &); //fixing "KDE kogout was cancelled by zNotes" bug
	bool hasRanCopy();
private slots:
	void copyStarted();
private:
	QWidget* main_window;
};

#endif // APPLICATION_H
