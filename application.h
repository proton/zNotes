#ifndef APPLICATION_H
#define APPLICATION_H

#include <QtGui/QApplication>

class QtLocalPeer;

#ifdef SINGLE_INSTANCE
#if defined(Q_WS_WIN)
#  if !defined(QT_QTSINGLEAPPLICATION_EXPORT) && !defined(QT_QTSINGLEAPPLICATION_IMPORT)
#    define QT_QTSINGLEAPPLICATION_EXPORT
#  elif defined(QT_QTSINGLEAPPLICATION_IMPORT)
#    if defined(QT_QTSINGLEAPPLICATION_EXPORT)
#      undef QT_QTSINGLEAPPLICATION_EXPORT
#    endif
#    define QT_QTSINGLEAPPLICATION_EXPORT __declspec(dllimport)
#  elif defined(QT_QTSINGLEAPPLICATION_EXPORT)
#    undef QT_QTSINGLEAPPLICATION_EXPORT
#    define QT_QTSINGLEAPPLICATION_EXPORT __declspec(dllexport)
#  endif
#else
#  define QT_QTSINGLEAPPLICATION_EXPORT
#endif
#endif

class zApplication: public QApplication
{
	Q_OBJECT
public:
	zApplication(int &argc, char **argv);
	~zApplication() {}
	void commitData(QSessionManager &); //fixing "KDE kogout was cancelled by zNotes" bug
	inline void setMainWindow(QWidget* widget) { main_window = widget; }
	bool sendMessage(const QString &message, int timeout = 5000);
private slots:
	void copyStarted(const QString&);
private:
	QWidget* main_window;
	void _init(const QString &appId = QString());
#ifdef SINGLE_INSTANCE
	QtLocalPeer* peer;
#endif
};

#endif // APPLICATION_H
