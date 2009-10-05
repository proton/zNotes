#include "settings.h"

#include <QSettings>
#include <QObject>

QSettings settings("pDev", "zNotes");

QString NotesPath;
bool HideStart;

void Settings::Load()
{
	NotesPath = settings.value("NotesPath").toString();
	HideStart = settings.value("HideStart").toBool();
}

const QString& Settings::getNotesPath()
{
	return NotesPath;
}

bool Settings::getHideStart()
{
	return HideStart;
}

void Settings::setNotesPath(const QString& path)
{
	if(NotesPath != path)
	{
		NotesPath = path;
		settings.setValue("NotesPath", NotesPath);
	}
}

void Settings::setHideStart(bool hide)
{
	if(HideStart != hide)
	{
		HideStart = hide;
		settings.setValue("HideStart", HideStart);
	}
}
