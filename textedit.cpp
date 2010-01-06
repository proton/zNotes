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

template<> TextEdit<QTextEdit>::TextEdit()
	: QTextEdit()
{
	highlighter = new Highlighter(QTextEdit::document());
	QTextEdit::connect(&settings, SIGNAL(NoteHighlightChanged()), highlighter, SLOT(rehighlight()));
}

template<> TextEdit<QTextBrowser>::TextEdit()
	: QTextBrowser()
{
	highlighter = new Highlighter(QTextBrowser::document());
	QTextBrowser::connect(&settings, SIGNAL(NoteHighlightChanged()), highlighter, SLOT(rehighlight()));
	//setReadOnly(false);
	QTextBrowser::page()->setContentEditable(true);
	setOpenLinks(true);
	setOpenExternalLinks(true);
}

template<> const QString TextEdit<QTextEdit>::text() const
{
	return QTextEdit::toPlainText();
}

template<> void TextEdit<QTextEdit>::setText(const QString& text)
{
	setPlainText(text);
}

template<> const QString TextEdit<QTextBrowser>::text() const
{
	return QTextBrowser::toHtml();
}

template<> void TextEdit<QTextBrowser>::setText(const QString& text)
{
	QTextBrowser::setHtml(text);
}

//Mouse pressing
template<class T> void TextEdit<T>::mousePressEvent(QMouseEvent *e)
{
	if(settings.getNoteLinksOpen() && e->buttons()==Qt::LeftButton && e->modifiers()&Qt::ControlModifier) //Ctrl+LeftMouseButton
	{
		bool onLink = false;
		int position_start, position_end;
		const QTextCursor cursor = T::cursorForPosition(e->pos());
		onLink = isOnLink(*T::document(), cursor, position_start, position_end);
		//
		if(onLink) //if link under cursor
		{
			const QUrl link(T::toPlainText().mid(position_start, position_end-position_start));
			QDesktopServices::openUrl(link);
		}
	}
	else T::mousePressEvent(e);
}

//Mouse moving
template<class T> void TextEdit<T>::mouseMoveEvent(QMouseEvent *e)
{
	bool onLink = false;
	int position_start, position_end;
	if(settings.getNoteLinksOpen() && e->buttons()==Qt::NoButton && e->modifiers()&Qt::ControlModifier) //Ctrl+NoMouseButton
	{
		QTextCursor cursor = T::cursorForPosition(e->pos());
		onLink = isOnLink(*T::document(), cursor, position_start, position_end);
	}
	if(onLink) //if link under cursor
	{
		QTextEdit::ExtraSelection sel;
		sel.cursor = T::textCursor();
		sel.cursor.setPosition(position_start);
		sel.cursor.setPosition(position_end, QTextCursor::KeepAnchor);
		sel.format = highlighter->linkFormat;
		sel.format.setFontUnderline(true);
		T::setExtraSelections(QList<QTextEdit::ExtraSelection>() << sel); //Underlining link
		T::viewport()->setCursor(Qt::PointingHandCursor);
	}
	else
	{
		T::setExtraSelections(QList<QTextEdit::ExtraSelection>()); //Clearing
		T::viewport()->setCursor(Qt::IBeamCursor);
	}
	T::mouseMoveEvent(e);
}

//Focus out
template<class T> void TextEdit<T>::focusOutEvent(QFocusEvent*)
{
	T::setExtraSelections(QList<QTextEdit::ExtraSelection>()); //Clearing
}
