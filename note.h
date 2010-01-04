#ifndef NOTE_H
#define NOTE_H

#include <QTextEdit>
#include <QTextBrowser>
#include <QFile>
#include <QDir>

#include "highlighter.h"

template<class T> class TextEdit : public T
{
public:
	TextEdit();
private:
	Highlighter* highlighter;
	//
	void mousePressEvent(QMouseEvent *e);
	void mouseMoveEvent(QMouseEvent *e);
	void focusOutEvent(QFocusEvent *e);
};

template<> class TextEdit<QTextEdit> : public QTextEdit
{
	void initialize();
public:
	inline const QString text() const
	{
		return QTextEdit::toPlainText();
	}
	inline void setText(const QString& text)
	{
		QTextEdit::setPlainText(text);
	}
};

template<> class TextEdit<QTextBrowser> : public QTextBrowser
{
	void initialize();
public:
	inline const QString text() const
	{
		return QTextBrowser::toHtml();
	}
	inline void setText(const QString& text)
	{
		QTextBrowser::setHtml(text);
	}
};


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
	void load(); //Reading note's content
	void save(); //Saving note's content
	void rename(const QString& new_name);
	void move(const QString& new_dir);
	bool remove(); //Removing note from harddisk
	//
	void copy() const;
	bool find(const QString& text, bool next=false);
	//
	void setSelFormat(const QTextCharFormat& format);
	const QTextCharFormat getSelFormat() const;
private:
	Type type;
	QString note_title;
	QFileInfo file_info;
	QFile file;
	//
	TextEdit<QTextEdit>* text_edit;
	TextEdit<QTextBrowser>* html_edit;
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
