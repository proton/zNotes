#include "textedit.h"
#include "settings.h"

#include <QRegExp>
#include <QMouseEvent>
#include <QUrl>
#include <QDesktopServices>

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
		QStringList signlist;
		signlist << "http://" << "https://" << "ftp://";
		const int link_lenght = pos_end-pos_start;
		QStringList::const_iterator sign_it;
		for(sign_it=signlist.constBegin(); sign_it!=signlist.constEnd(); ++sign_it)
		{
			if(link_lenght>sign_it->size())
			{
				bool isLink = true;
				int i = 0;
				while(i<sign_it->size())
				{
					if(document.characterAt(pos_start+i)!=sign_it->at(i))
					{
						isLink = false;
						break;
					}
					++i;
				}
				if(isLink) return true;
			}
		}
	}
	return false;
}

//------------------------------------------------------------------------------

TextEdit::TextEdit()
	: QTextEdit()
{
	highlighter = new Highlighter(document());
	connect(&settings, SIGNAL(NoteHighlightChanged()), highlighter, SLOT(rehighlight()));
	setFont(settings.getNoteFont());
	connect(&settings, SIGNAL(NoteFontChanged()), this, SLOT(fontChanged()));
	setMouseTracking(settings.getNoteLinksOpen());
	connect(&settings, SIGNAL(NoteLinkOpenChanged()), this, SLOT(linkOpenChanged()));
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
	else QTextEdit::mousePressEvent(e);
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
	QTextEdit::mouseMoveEvent(e);
}

//Focus out
void TextEdit::focusOutEvent(QFocusEvent*)
{
	setExtraSelections(QList<QTextEdit::ExtraSelection>()); //Clearing
}

//If notes' font changed in the preferences, applying font to note
void TextEdit::fontChanged()
{
	setFont(settings.getNoteFont());
}

void TextEdit::linkOpenChanged()
{
	bool is_link_open = settings.getNoteLinksOpen();
	setMouseTracking(is_link_open);
	if(!is_link_open)
	{
		setExtraSelections(QList<QTextEdit::ExtraSelection>());
		viewport()->setCursor(Qt::IBeamCursor);
	}
}

//Searching
bool TextEdit::search(const QString& text, bool next)
{
	if(next) //search next
	{
		QTextCursor cursor(textCursor().block().next());
		setTextCursor(cursor);
	}
	else unsetCursor(); //new search
	return find(text);
}

