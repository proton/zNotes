#ifndef SETTINGS_H
#define SETTINGS_H

#include <QString>
#include <QObject>
#include <QSettings>
#include <QRect>
#include <QPoint>
#include <QByteArray>
#include <QFont>
#include <QVector>

#include "scriptmodel.h"
#include "toolbaraction.h"

struct Script
{
	QString name;
	QString icon;
	QString addr;
};

class Settings : public QObject
{
	Q_OBJECT
public:
	Settings();
	//
	inline const QString& getNotesPath()			{ return NotesPath; }
	inline const QString& getLastNote()				{ return LastNote; }
	inline bool getHideStart()						{ return HideStart; }
	inline const QByteArray& getDialogGeometry()	{ return DialogGeometry; }
	inline const QByteArray& getDialogState()		{ return DialogState; }
	inline bool getHideToolbar()					{ return HideToolbar; }
	inline bool getHideFrame()						{ return HideFrame; }
	inline bool getStayTop()						{ return StayTop; }
	inline const QFont& getNoteFont()				{ return NoteFont; }
	inline ScriptModel& getScriptModel()			{ return smodel; }
	inline bool getScriptShowOutput()				{ return ScriptShowOutput; }
	inline bool getScriptCopyOutput()				{ return ScriptCopyOutput; }
	inline const QVector<int>& getTbItems()			{ return tb_items; }
	//
	void setNotesPath(const QString& path);
	void setLastNote(const QString& name);
	void setHideStart(bool hide);
	void setDialogGeometry(const QByteArray& g);
	void setDialogState(const QByteArray& g);
	void setHideToolbar(bool s);
	void setHideFrame(bool s);
	void setStayTop(bool s);
	void setNoteFont(const QFont& f);
	void setScriptShowOutput(bool b);
	void setScriptCopyOutput(bool b);
	void setScripts();
private:
	QSettings config;
	//
	QString NotesPath;
	QString LastNote;
	bool HideStart;
	//
	QByteArray DialogGeometry;
	QByteArray DialogState;
	//
	bool HideToolbar;
	bool HideFrame;
	bool StayTop;
	//
	QFont NoteFont;
	//
	ScriptModel smodel;
	bool ScriptShowOutput;
	bool ScriptCopyOutput;
	//
	QVector<int> tb_items;
	//
	inline const char* getItemName(int i)
	{
		switch(i)
		{
			case itemAdd: return "Toolbar/itemAdd";
			case itemRemove: return "Toolbar/itemRemove";
			case itemRename: return "Toolbar/itemRename";
			case itemPrev: return "Toolbar/itemPrev";
			case itemNext: return "Toolbar/itemNext";
			case itemCopy: return "Toolbar/itemCopy";
			case itemSetup: return "Toolbar/itemSetup";
			case itemInfo: return "Toolbar/itemInfo";
			case itemRun: return "Toolbar/itemRun";
			case itemSearch: return "Toolbar/itemSearch";
			case itemExit: return "Toolbar/itemExit";
			default: return "";
		}
	}
signals:
	void NotesPathChanged();
	void WindowStateChanged();
	void ToolbarVisChanged();
	void NoteFontChanged();
	void tbHidingChanged();
};

extern Settings settings;

#endif // SETTINGS_H
