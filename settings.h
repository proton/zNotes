#ifndef SETTINGS_H
#define SETTINGS_H

#include <QString>
#include <QObject>
#include <QSettings>

class Settings : public QObject
{
	Q_OBJECT
public:
	Settings();
	//
	const QString& getNotesPath();
	const QString& getLastNote();
	bool getHideStart();
	//
	void setNotesPath(const QString& path);
	void setLastNote(const QString& name);
	void setHideStart(bool hide);
private:
	QSettings config;
	//
	QString NotesPath;
	QString LastNote;
	bool HideStart;
signals:
	void NotesPathChanged();
};

extern Settings settings;

#endif // SETTINGS_H
