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
private slots:
	void fontChanged();
	void linkOpenChanged();
};

#endif // TEXTEDIT_H
