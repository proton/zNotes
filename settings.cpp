#include "settings.h"

#include <QObject>

Settings::Settings() : config("pDev", "zNotes")
{
	NotesPath = config.value("NotesPath").toString();
	LastNote = config.value("LastNote").toString();
	HideStart = config.value("HideStart").toBool();
}

const QString& Settings::getNotesPath()
{
	return NotesPath;
}

const QString& Settings::getLastNote()
{
	return LastNote;
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
		config.setValue("NotesPath", NotesPath);
		emit NotesPathChanged();
	}
}

void Settings::setLastNote(const QString& note_name)
{
	if(LastNote != note_name)
	{
		LastNote = note_name;
		config.setValue("LastNote", LastNote);
	}
}

void Settings::setHideStart(bool hide)
{
	if(HideStart != hide)
	{
		HideStart = hide;
		config.setValue("HideStart", HideStart);
	}
}
