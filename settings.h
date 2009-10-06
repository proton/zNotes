#ifndef SETTINGS_H
#define SETTINGS_H

#include <QString>
#include <QObject>
#include <QSettings>
#include <QRect>
#include <QPoint>

class Settings : public QObject
{
	Q_OBJECT
public:
	Settings();
	//
	const QString& getNotesPath();
	const QString& getLastNote();
	bool getHideStart();
	const QRect& getDialogRect();
	const QPoint& getDialogPos();
	//
	void setNotesPath(const QString& path);
	void setLastNote(const QString& name);
	void setHideStart(bool hide);
	void setDialogRect(const QRect& r);
	void setDialogPos(const QPoint& p);
private:
	QSettings config;
	//
	QString NotesPath;
	QString LastNote;
	bool HideStart;
	QRect DialogRect;
	QPoint DialogPos;
signals:
	void NotesPathChanged();
};

extern Settings settings;

#endif // SETTINGS_H
