#include "settings.h"

#include <QObject>
#include <QtDebug>

Settings::Settings() : config("pDev", "zNotes")
{
	NotesPath = config.value("NotesPath").toString();
	LastNote = config.value("LastNote").toString();
	HideStart = config.value("HideStart").toBool();
	DialogPos.setX(config.value("DialogX").toInt());
	DialogPos.setY(config.value("DialogY").toInt());
	DialogRect.setWidth(config.value("DialogWidth").toInt());
	DialogRect.setHeight(config.value("DialogHeight").toInt());
	if(DialogRect.width()==0) DialogRect.setWidth(300);
	if(DialogRect.height()==0) DialogRect.setHeight(400);
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

const QRect& Settings::getDialogRect()
{
	return DialogRect;
}

const QPoint& Settings::getDialogPos()
{
	return DialogPos;
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

void Settings::setDialogRect(const QRect& r)
{
	DialogRect = r;
	config.setValue("DialogWidth", r.width());
	config.setValue("DialogHeight", r.height());

}

void Settings::setDialogPos(const QPoint& p)
{
	DialogPos = p;
	config.setValue("DialogX", p.x());
	config.setValue("DialogY", p.y());

}
