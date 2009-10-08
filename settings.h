#ifndef SETTINGS_H
#define SETTINGS_H

#include <QString>
#include <QObject>
#include <QSettings>
#include <QRect>
#include <QPoint>
#include <QByteArray>
#include <QFont>

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
	inline bool getHideToolbar()					{ return HideToolbar; }
	inline bool getHideFrame()						{ return HideFrame; }
	inline bool getStayTop()						{ return StayTop; }
	inline const QFont& getNoteFont()				{ return NoteFont; }
	//
	void setNotesPath(const QString& path);
	void setLastNote(const QString& name);
	void setHideStart(bool hide);
	void setDialogGeometry(const QByteArray& g);
	void setHideToolbar(bool s);
	void setHideFrame(bool s);
	void setStayTop(bool s);
	void setNoteFont(const QFont& f);
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
	//
	QFont NoteFont;
signals:
	void NotesPathChanged();
	void WindowStateChanged();
	void ToolbarVisChanged();
	void NoteFontChanged();
};

extern Settings settings;

#endif // SETTINGS_H
