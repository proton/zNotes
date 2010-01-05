#include "textedit.h"
#include "settings.h"

#include <QRegExp>
#include <QMouseEvent>
#include <QUrl>
#include <QDesktopServices>

#include <QtDebug>

inline bool isOnLink(const QTextDocument& document, const QTextCursor& cursor, int& pos_start, int& pos_end)
{
	int position = cursor.position();
	if(!document.characterAt(position).isSpace())
	{
		pos_start = position;
		while(pos_start>0 && !document.characterAt(pos_start-1).isSpace()) --pos_start;
		const int characterCount = document.characterCount();
		pos_end = position;
		while(pos_end<characterCount && !document.characterAt(pos_end).isSpace()) ++pos_end;
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

template<class T> TextEdit<T>::TextEdit()
	: T()
{
	qDebug() << "TextEdit<T>::TextEdit()";
	highlighter = new Highlighter(T::document());
	T::connect(&settings, SIGNAL(NoteHighlightChanged()), highlighter, SLOT(rehighlight()));
	TextEdit<T>::initialize();
}

template<> void TextEdit<QTextEdit>::initialize()
{
	qDebug() << "TextEdit<QTextEdit>::initialize()";
}

template<> const QString TextEdit<QTextEdit>::text() const
{
	qDebug() << "TextEdit<QTextEdit>::text()";
	return QTextEdit::toPlainText();
}

template<> void TextEdit<QTextEdit>::setText(const QString& text)
{
	setPlainText(text);
}

template<> void TextEdit<QTextBrowser>::initialize()
{
	qDebug() << "TextEdit<QTextEdit>::initialize()";
	setReadOnly(false);
	setOpenLinks(true);
	setOpenExternalLinks(true);
}

template<> const QString TextEdit<QTextBrowser>::text() const
{
	qDebug() << "TextEdit<QTextBrowser>::text()";
	return QTextBrowser::toHtml();
}

template<> void TextEdit<QTextBrowser>::setText(const QString& text)
{
	QTextBrowser::setHtml(text);
}

template<class T> void TextEdit<T>::mousePressEvent(QMouseEvent *e)
{
	if(settings.getNoteLinksOpen() && e->buttons()==Qt::LeftButton && e->modifiers()&Qt::ControlModifier) //Ctrl+LeftMouseButton
	{
		bool onLink = false;
		int position_start, position_end;
		if(e->buttons()==Qt::NoButton && e->modifiers()&Qt::ControlModifier)
		{
			const QTextCursor cursor = T::cursorForPosition(e->pos());
			onLink = isOnLink(*T::document(), cursor, position_start, position_end);
		}
		if(onLink)
		{
			const QUrl link(T::toPlainText().mid(position_start, position_end));
			QDesktopServices::openUrl(link);
		}
	}
	else T::mousePressEvent(e);
}

template<class T> void TextEdit<T>::mouseMoveEvent(QMouseEvent *e)
{
	bool onLink = false;
	int position_start, position_end;
	if(settings.getNoteLinksOpen() && e->buttons()==Qt::NoButton && e->modifiers()&Qt::ControlModifier)
	{
		const QTextCursor cursor = T::cursorForPosition(e->pos());
		onLink = isOnLink(*T::document(), cursor, position_start, position_end);
	}
	if(onLink)
	{
		QTextEdit::ExtraSelection sel;
		sel.cursor = T::textCursor();
		sel.cursor.setPosition(position_start);
		sel.cursor.setPosition(position_end, QTextCursor::KeepAnchor);
		sel.format = highlighter->linkFormat;
		sel.format.setFontUnderline(true);
		T::setExtraSelections(QList<QTextEdit::ExtraSelection>() << sel);
		T::viewport()->setCursor(Qt::PointingHandCursor);
	}
	else
	{
		T::setExtraSelections(QList<QTextEdit::ExtraSelection>());
		T::viewport()->setCursor(Qt::IBeamCursor);
	}
	T::mouseMoveEvent(e);
}

template<class T> void TextEdit<T>::focusOutEvent(QFocusEvent*)
{
	T::setExtraSelections(QList<QTextEdit::ExtraSelection>());
}
