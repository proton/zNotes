#ifndef NOTE_H
#define NOTE_H

#include "textedit.h"

#include <QFile>

class Note : public QObject
{
	Q_OBJECT
public:
	enum Type { type_text, type_html };
public:
	Note(const QFileInfo& fileinfo);
	~Note();
	//
	inline const QString& title() { return note_title; }
	inline const QString absolutePath() { return file_info.absoluteFilePath(); }
	inline Type noteType() { return type; }
	//
	QWidget* widget();
	//
	void setTitle(bool show_extensions);
	void load(); //Reading note's content
	void save(bool forced = false); //Saving note's content
	void rename(const QString& new_name);
	void move(const QString& new_dir);
	bool remove(); //Removing note from harddisk
	//
	void copy() const; //Coping note's content to clipboard
	bool find(const QString& text, bool next=false); //Searching text in a note's content
	//
	void setSelFormat(const QTextCharFormat& format);
	const QTextCharFormat getSelFormat() const;
private:
	Type type;
	QString note_title;
	QFileInfo file_info;
	QFile file;
	//
	TextEdit* text_edit;
	//QWebView* html_edit;
	//
	bool content_changed;
private slots:
	void contentChanged();
	void noteFontChanged();
	void noteLinkOpenChanged();
	//
	void currentCharFormatChanged(const QTextCharFormat&);
signals:
	void formatChanged(const QFont& font);
};

#endif // NOTE_H
