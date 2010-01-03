#ifndef NOTE_H
#define NOTE_H

#include <QTextEdit>
#include <QFile>
#include <QDir>

#include "highlighter.h"

class TextEdit : public QTextEdit
{
public:
	enum TextType { type_text, type_html };
public:
	TextEdit(TextType new_type);
	//
	inline const QString text() const
	{
		switch(type)
		{
			case type_text: return toPlainText();
			case type_html: return toHtml();
		}
	}
	inline void setText(const QString& text)
	{
		switch(type)
		{
			case type_text: return setPlainText(text);
			case type_html: return setHtml(text);
		}
	}
private:
	TextType type;
	Highlighter* highlighter;
	//
	void mousePressEvent(QMouseEvent *e);
	void mouseMoveEvent(QMouseEvent *e);
	void focusOutEvent(QFocusEvent *e);
};

class Note : public QObject
{
	Q_OBJECT
private:
	enum Type { type_text, type_html };
public:
	Note(const QFileInfo& fileinfo);
	~Note();
	//
	inline const QString& title() { return note_title; }
	inline const QString absolutePath() { return file_info.absoluteFilePath(); }
	//
	QWidget* widget();
	//
	void load();
	void save();
	void rename(const QString& new_name);
	bool remove();
	//
	void copy();
	bool find(const QString& text);
private:
	Type type;
	QString note_title;
	QFileInfo file_info;
	QFile file;
	//
	TextEdit* text_edit;
	//
	bool content_changed;
private slots:
	void contentChanged();
	void noteFontChanged();
	void noteLinkOpenChanged();
};

#endif // NOTE_H
