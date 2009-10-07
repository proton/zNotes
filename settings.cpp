#include "settings.h"

#include <QObject>
#include <QtDebug>

Settings::Settings() : config("pDev", "zNotes")
{
	NotesPath = config.value("NotesPath").toString();
	LastNote = config.value("LastNote").toString();
	HideStart = config.value("HideStart").toBool();
	DialogGeometry = config.value("DialogGeometry").toByteArray();
	//
	HideToolbar = config.value("HideToolbar").toBool();
	HideFrame = config.value("HideFrame").toBool();
	StayTop = config.value("StayTop").toBool();
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

const QByteArray& Settings::getDialogGeometry()
{
	return DialogGeometry;
}

bool Settings::getHideToolbar()
{
	return HideToolbar;
}

bool Settings::getHideFrame()
{
	return HideFrame;
}

bool Settings::getStayTop()
{
	return StayTop;
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

void Settings::setHideToolbar(bool Hide)
{
	if(HideToolbar != Hide)
	{
		HideToolbar = Hide;
		config.setValue("HideToolbar", HideToolbar);
		emit ToolbarVisChanged();
	}
}

void Settings::setHideFrame(bool Hide)
{
	if(HideFrame != Hide)
	{
		HideFrame = Hide;
		config.setValue("HideFrame", HideToolbar);
		emit WindowStateChanged();
	}
}

void Settings::setStayTop(bool top)
{
	if(StayTop != top)
	{
		StayTop = top;
		config.setValue("StayTop", StayTop);
		emit WindowStateChanged();
	}
}

void Settings::setDialogGeometry(const QByteArray& g)
{
	DialogGeometry = g;
	config.setValue("DialogGeometry", DialogGeometry);
}
