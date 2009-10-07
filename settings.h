#ifndef SETTINGS_H
#define SETTINGS_H

#include <QString>
#include <QObject>
#include <QSettings>
#include <QRect>
#include <QPoint>
#include <QByteArray>

class Settings : public QObject
{
	Q_OBJECT
public:
	Settings();
	//
	const QString& getNotesPath();
	const QString& getLastNote();
	bool getHideStart();
	const QByteArray& getDialogGeometry();
	bool getHideToolbar();
	bool getHideFrame();
	bool getStayTop();
	//
	void setNotesPath(const QString& path);
	void setLastNote(const QString& name);
	void setHideStart(bool hide);
	void setDialogGeometry(const QByteArray& g);
	void setHideToolbar(bool s);
	void setHideFrame(bool s);
	void setStayTop(bool s);
private:
	QSettings config;
	//
	QString NotesPath;
	QString LastNote;
	bool HideStart;
	//
	QByteArray DialogGeometry;
	//
	bool HideToolbar;
	bool HideFrame;
	bool StayTop;

signals:
	void NotesPathChanged();
	void WindowStateChanged();
	void ToolbarVisChanged();
};

extern Settings settings;

#endif // SETTINGS_H
