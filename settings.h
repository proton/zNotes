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
#include <QTranslator>
#include <QLocale>

#include <QtDebug>

#include "scriptmodel.h"

enum TabPosition { North, South, West, East };

class Settings : public QObject
{
	Q_OBJECT
public:
	Settings();
	void load();
	//
	inline const QString& getNotesPath()			{ return notes_path; }
	inline const QString& getLastNote()				{ return last_note; }
	inline bool getHideStart()						{ return hide_start; }
	inline const QByteArray& getDialogGeometry()	{ return dialog_geometry; }
	inline const QByteArray& getDialogState()		{ return dialog_state; }
	inline TabPosition getTabPosition()				{ return tab_position; }
	inline bool getShowHidden()						{ return show_hidden; }
	inline bool getShowExtensions()					{ return show_extensions; }
	inline bool getHideFrame()						{ return hide_frame; }
	inline bool getStayTop()						{ return stay_top; }
	inline bool getSingleInstance()					{ return single_instance; }
	inline bool getCopyStartRaise()					{ return copy_start_raise; }
	inline bool getFileScanner()					{ return file_scanner; }
	inline int getFileScannerTimeout()				{ return file_scanner_timeout; }
	inline const QFont& getNoteFont()				{ return note_font; }
	inline ScriptModel& getScriptModel()			{ return script_model; }
	inline bool getScriptShowOutput()				{ return script_show_output; }
	inline bool getScriptCopyOutput()				{ return script_copy_output; }
	inline const QVector<int>& getToolbarItems()	{ return tb_items; }
	inline const QMap<QLocale::Language, QString>& getTranslations()	{ return translations; }
	inline QLocale::Language getLanguageCurrent()	{ return language_current; }
	inline bool getLanguageCustom()					{ return language_custom; }
	inline bool getNoteLinksHighlight()				{ return note_links_highlight; }
	inline bool getNoteLinksOpen()					{ return note_links_open; }
	//
	void setNotesPath(const QString& path);
	void setLastNote(const QString& name);
	void setHideStart(bool hide);
	void setDialogGeometry(const QByteArray& g);
	void setDialogState(const QByteArray& g);
	void setTabPosition(TabPosition v);
	void setShowHidden(bool s);
	void setShowExtensions(bool s);
	void setHideFrame(bool s);
	void setStayTop(bool s);
	void setSingleInstance(bool s);
	void setCopyStartRaise(bool s);
	void setFileScanner(bool v);
	void setFileScannerTimeout(int v);
	void setNoteFont(const QFont& f);
	void setScriptShowOutput(bool b);
	void setScriptCopyOutput(bool b);
	void setScripts();
	void setToolbarItems(const QVector<int>& v);
	void setLanguage(QLocale::Language);
	void setLanguageCurrent(QLocale::Language);
	void setLanguageCustom(bool);
	void setNoteLinksHighlight(bool);
	void setNoteLinksOpen(bool);
	//
	void loadLanguages();
	//
	inline void save() { config.sync(); }
private:
	QSettings config;
	QTranslator translator;
	QTranslator qtranslator;
	//
	QMap<QLocale::Language, QString> translations;
	QLocale::Language language_current;
	bool language_custom;
	//
	QString notes_path;
	QString last_note;
	bool hide_start;
	//
	QByteArray dialog_geometry;
	QByteArray dialog_state;
	//
	TabPosition tab_position;
	bool show_hidden;
	bool show_extensions;
	bool hide_frame;
	bool stay_top;
	//
	bool single_instance;
	bool copy_start_raise;
	//
	bool file_scanner;
	int file_scanner_timeout;
	//
	QFont note_font;
	bool note_links_highlight;
	bool note_links_open;
	//
	ScriptModel script_model;
	bool script_show_output;
	bool script_copy_output;
	//
	QVector<int> tb_items;
	//
	QLocale::Language system_language;
signals:
	void NotesPathChanged();
	void FileScannerEnChanged(bool);
	void FileScannerTimeoutChanged(int);
	void ShowExtensionsChanged(bool show_extensions);
	void ShowHiddenChanged();
	void TabPositionChanged();
	void WindowStateChanged();
	void NoteFontChanged();
	void NoteHighlightChanged();
	void NoteLinkOpenChanged();
	void tbHidingChanged();
	void ToolbarItemsChanged();
};

extern Settings settings;

#endif // SETTINGS_H
