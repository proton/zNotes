#ifndef TEXTEDIT_H
#define TEXTEDIT_H

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
	//
	void initialize();
public:
	const QString text() const;
	void setText(const QString& text);
};

#endif // TEXTEDIT_H
