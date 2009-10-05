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
	int getDialogWidth();
	int getDialogHeight();
	//
	void setNotesPath(const QString& path);
	void setLastNote(const QString& name);
	void setHideStart(bool hide);
	void setDialogWidth(int w);
	void setDialogHeight(int h);
private:
	QSettings config;
	//
	QString NotesPath;
	QString LastNote;
	bool HideStart;
	int DialogWidth, DialogHeight;
signals:
	void NotesPathChanged();
};

extern Settings settings;

#endif // SETTINGS_H
