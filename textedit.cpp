#include "textedit.h"
#include "settings.h"

#include <QRegExp>
#include <QMouseEvent>
#include <QUrl>
#include <QDesktopServices>

#include <QtDebug>

//Checking note's text for link
inline bool isOnLink(const QTextDocument& document, const QTextCursor& cursor, int& pos_start, int& pos_end)
{
	const int position = cursor.position();
	if(!document.characterAt(position).isSpace())
	{
		pos_start = position;
		while(pos_start>0 && !document.characterAt(pos_start-1).isSpace()) --pos_start; //Detecting start on the word
		const int characterCount = document.characterCount();
		pos_end = position;
		while(pos_end<characterCount && !document.characterAt(pos_end).isSpace()) ++pos_end; //Detecting end on the word
		//
		const QString priznak("http://");
		const int link_lenght = pos_end-pos_start;
		if(link_lenght>priznak.size())
		{
			bool isLink = true;
			int i = 0;
			while(i<priznak.size())
			{
				if(document.characterAt(pos_start+i)!=priznak.at(i))
				{
					isLink = false;
					break;
				}
				++i;
			}
			if(isLink) return true;
		}
	}
	return false;
}

//------------------------------------------------------------------------------

TextEdit::TextEdit(TextType new_type)
	: QTextEdit(), type(new_type)
{
	highlighter = new Highlighter(document());
	connect(&settings, SIGNAL(NoteHighlightChanged()), highlighter, SLOT(rehighlight()));
}

//Mouse pressing
void TextEdit::mousePressEvent(QMouseEvent *e)
{
	if(settings.getNoteLinksOpen() && e->buttons()==Qt::LeftButton && e->modifiers()&Qt::ControlModifier) //Ctrl+LeftMouseButton
	{
		bool onLink = false;
		int position_start, position_end;
		const QTextCursor cursor = cursorForPosition(e->pos());
		onLink = isOnLink(*document(), cursor, position_start, position_end);
		//
		if(onLink) //if link under cursor
		{
			const QUrl link(toPlainText().mid(position_start, position_end-position_start));
			QDesktopServices::openUrl(link);
		}
	}
	else mousePressEvent(e);
}

//Mouse moving
void TextEdit::mouseMoveEvent(QMouseEvent *e)
{
	bool onLink = false;
	int position_start, position_end;
	if(settings.getNoteLinksOpen() && e->buttons()==Qt::NoButton && e->modifiers()&Qt::ControlModifier) //Ctrl+NoMouseButton
	{
		QTextCursor cursor = cursorForPosition(e->pos());
		onLink = isOnLink(*document(), cursor, position_start, position_end);
	}
	if(onLink) //if link under cursor
	{
		QTextEdit::ExtraSelection sel;
		sel.cursor = textCursor();
		sel.cursor.setPosition(position_start);
		sel.cursor.setPosition(position_end, QTextCursor::KeepAnchor);
		sel.format = highlighter->linkFormat;
		sel.format.setFontUnderline(true);
		setExtraSelections(QList<QTextEdit::ExtraSelection>() << sel); //Underlining link
		viewport()->setCursor(Qt::PointingHandCursor);
	}
	else
	{
		setExtraSelections(QList<QTextEdit::ExtraSelection>()); //Clearing
		viewport()->setCursor(Qt::IBeamCursor);
	}
	mouseMoveEvent(e);
}

//Focus out
void TextEdit::focusOutEvent(QFocusEvent*)
{
	setExtraSelections(QList<QTextEdit::ExtraSelection>()); //Clearing
}
