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
#include <QMap>

#include "scriptmodel.h"

struct HighlightRule
{
	HighlightRule(bool e, const QString& r, const QColor& c)
		: enabled(e), regexp(r), color(c) {}
	HighlightRule() {}
	bool enabled;
	QString regexp;
	QColor color;
	inline bool operator == (const HighlightRule& hr) const
	{
		if(enabled!=hr.enabled) return false;
		if(regexp!=hr.regexp) return false;
		if(color!=hr.color) return false;
		return true;
	}
};

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
	inline const QFont& getNoteFont()				{ return note_font; }
	inline ScriptModel& getScriptModel()			{ return script_model; }
	inline bool getScriptShowOutput()				{ return script_show_output; }
	inline bool getScriptCopyOutput()				{ return script_copy_output; }
	inline const QVector<int>& getToolbarItems()	{ return tb_items; }
	inline bool getNoteHighlight()					{ return note_highlight; }
	inline const QVector<HighlightRule>& getHighlightRules()	{ return highlight_rules; }
	inline const QMap<int, QMap<int, QString> >& getTranslations()	{ return translations; }
	inline const QLocale& getLocaleCurrent()		{ return locale_current; }
	inline const QLocale& getLocale()				{ return locale; }
	inline bool getLanguageCustom()					{ return language_custom; }
	inline bool getNoteLinksOpen()					{ return note_links_open; }
	inline bool getNotePastePlaintext()				{ return note_paste_plaintext; }
	//
	void setNotesPath(const QString& path);
	void setLastNote(const QString& name);
	void setHideStart(bool hide);
	void setDialogGeometry(const QByteArray& g);
	void setDialogState(const QByteArray& g);
	void setTabPosition(TabPosition v);
	void setShowHidden(bool s);
	void setShowExtensions(bool s);
	void setHideFrame(bool);
	void setStayTop(bool);
	void setSingleInstance(bool);
	void setCopyStartRaise(bool);
	void setNoteFont(const QFont&);
	void setScriptShowOutput(bool);
	void setScriptCopyOutput(bool);
	void setScripts();
	void setToolbarItems(const QVector<int>&);
	void setNoteHighlight(bool);
	void setHighlightRules(const QVector<HighlightRule>&);
	void setLocaleCurrent(const QLocale&);
	void setLocaleCustom(bool);
	void setNoteLinksOpen(bool);
	void setNotePastePlaintext(bool);
	//
	void loadLanguages();
	void updateLocale();
	//
	inline void save() { config.sync(); }

private:
	QSettings config;
	QTranslator translator;
	QTranslator qtranslator;
	//
	QMap<int, QMap<int, QString> > translations;
    QMap<int, QMap<int, QString> > qtTranslations;
	QLocale locale_current;
	QLocale locale_system;
	QLocale locale;
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
	QFont note_font;
	bool note_links_open;
	bool note_paste_plaintext;
	//
	bool note_highlight;
	QVector<HighlightRule> highlight_rules;
	//
	ScriptModel script_model;
	bool script_show_output;
	bool script_copy_output;
	//
	QVector<int> tb_items;
signals:
	void NotesPathChanged();
	void ShowExtensionsChanged(bool show_extensions);
	void ShowHiddenChanged();
	void TabPositionChanged();
	void WindowStateChanged();
	void NoteFontChanged();
	void NoteHighlightChanged();
	void NoteLinkOpenChanged();
	void NotePastePlaintextChanged();
	void tbHidingChanged();
	void ToolbarItemsChanged();
};

extern Settings settings;

#endif // SETTINGS_H
