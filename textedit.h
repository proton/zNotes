#ifndef TEXTEDIT_H
#define TEXTEDIT_H

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
private:
	Highlighter* highlighter;
	TextType type;
	//
	void mousePressEvent(QMouseEvent *e);
	void mouseMoveEvent(QMouseEvent *e);
	void focusOutEvent(QFocusEvent *e);
public:
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
};

#endif // TEXTEDIT_H
