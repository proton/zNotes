#ifndef TEXTEDIT_H
#define TEXTEDIT_H

#include <QTextEdit>

#include "highlighter.h"

class TextEdit : public QTextEdit
{
	Q_OBJECT
public:
	TextEdit();
private:
	Highlighter* highlighter;
	//
	void mousePressEvent(QMouseEvent* e);
	void mouseMoveEvent(QMouseEvent* e);
	void focusOutEvent(QFocusEvent* e);
public:
	bool search(const QString& text, bool new_search=false); //Searching text in a note's content
private slots:
	void fontChanged();
	void linkOpenChanged();
};

#endif // TEXTEDIT_H
