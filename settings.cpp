#include "settings.h"

#include <QObject>
#include <QtDebug>

Settings::Settings() : config("pDev", "zNotes")
{
	NotesPath = config.value("NotesPath").toString();
	LastNote = config.value("LastNote").toString();
	HideStart = config.value("HideStart").toBool();
	DialogWidth = config.value("DialogWidth").toInt();
	DialogHeight = config.value("DialogHeight").toInt();
	if(DialogWidth==0) DialogWidth = 300;
	if(DialogHeight==0) DialogHeight = 400;
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

int Settings::getDialogWidth()
{
	return DialogWidth;
}

int Settings::getDialogHeight()
{
	return DialogHeight;
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

void Settings::setDialogWidth(int w)
{
	if(DialogWidth != w)
	{
		DialogWidth = w;
		config.setValue("DialogWidth", DialogWidth);
	}
}

void Settings::setDialogHeight(int h)
{
	if(DialogHeight != h)
	{
		DialogHeight = h;
		config.setValue("DialogHeight", DialogHeight);
	}
}
