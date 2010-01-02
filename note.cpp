#include <QTextStream>

#include <QRegExp>
#include <QtDebug>
#include <QMouseEvent>
#include <QUrl>
#include <QDesktopServices>

#include "note.h"

inline void text2links(QString& text)
{
	const QRegExp regexp("\\s");
	int start_index, last_index = 0;
	while((start_index = text.indexOf("http://", last_index)) != -1)
	{
		last_index = text.indexOf(regexp, start_index);
		if(last_index==-1) last_index = text.size();
		QString link(text.mid(start_index, last_index));
		QString html_link = QString("<a href='%1'>%1</a>").arg(link);
		text.replace(start_index, last_index, html_link);
		last_index+=(html_link.size()-link.size()+1);
	}
}

Note::Note(const QString& fn, const QDir& dir, const QFont& f)
	: QTextEdit(), name(fn), file(dir.absoluteFilePath(fn))
{
	setMouseTracking(true);// only is set open links
	highlighter = new Highlighter(this->document());
	if(file.open(QIODevice::ReadOnly | QIODevice::Text))
	{
		QTextStream in(&file);
		setPlainText(in.readAll());
//		QString content = in.readAll();
//		text2links(content);
//		qDebug() << content;
//		content.replace("\n","<br/>");
//		qDebug() << content;
//		setHtml(content);
		file.close();
	}
	else if(file.open(QIODevice::WriteOnly | QIODevice::Text)) file.close(); //If file don't exist, we creating itt
	setFont(f);
}

void Note::mousePressEvent(QMouseEvent *e)
{
	if(e->buttons()==Qt::LeftButton && e->modifiers()&Qt::ControlModifier) //Ctrl+LeftMouseButton
	{
		bool onLink = false;
		int position_start, position_end;
		if(e->buttons()==Qt::NoButton && e->modifiers()&Qt::ControlModifier)
		{
			const QTextCursor cursor = cursorForPosition(e->pos());
			//
			int position = cursor.position();
			if(!document()->characterAt(position).isSpace())
			{
				position_start = position;
				while(position_start>0 && !document()->characterAt(position_start-1).isSpace()) --position_start;
				const int characterCount = document()->characterCount();
				position_end = position;
				while(position_end<characterCount && !document()->characterAt(position_end).isSpace()) ++position_end;
				//
				const QString priznak("http://");
				if((position_end-position_start)>priznak.size())
				{
					bool isLink = true;
					int i = 0;
					while(i<priznak.size())
					{
						if(document()->characterAt(position_start+i)!=priznak.at(i))
						{
							isLink = false;
							break;
						}
						++i;
					}
					if(isLink) onLink=true;
				}
			}
		}
		if(onLink)
		{
			const QUrl link(toPlainText().mid(position_start, position_end));
			QDesktopServices::openUrl(link);
		}
	}
	else QTextEdit::mousePressEvent(e);
}

void Note::mouseMoveEvent(QMouseEvent *e)
{
	bool onLink = false;
	int position_start, position_end;
	if(e->buttons()==Qt::NoButton && e->modifiers()&Qt::ControlModifier)
	{
		const QTextCursor cursor = cursorForPosition(e->pos());
		//
		int position = cursor.position();
		if(!document()->characterAt(position).isSpace())
		{
			position_start = position;
			while(position_start>0 && !document()->characterAt(position_start-1).isSpace()) --position_start;
			const int characterCount = document()->characterCount();
			position_end = position;
			while(position_end<characterCount && !document()->characterAt(position_end).isSpace()) ++position_end;
			//
			const QString priznak("http://");
			if((position_end-position_start)>priznak.size())
			{
				bool isLink = true;
				int i = 0;
				while(i<priznak.size())
				{
					if(document()->characterAt(position_start+i)!=priznak.at(i))
					{
						isLink = false;
						break;
					}
					++i;
				}
				if(isLink) onLink=true;
			}
		}
	}
	if(onLink)
	{
		QTextEdit::ExtraSelection sel;
		sel.cursor = textCursor();
		sel.cursor.setPosition(position_start);
		sel.cursor.setPosition(position_end, QTextCursor::KeepAnchor);
		sel.format = highlighter->linkFormat;
		sel.format.setFontUnderline(true);
		setExtraSelections(QList<QTextEdit::ExtraSelection>() << sel);
		viewport()->setCursor(Qt::PointingHandCursor);
	}
	else
	{
		setExtraSelections(QList<QTextEdit::ExtraSelection>());
		viewport()->setCursor(Qt::IBeamCursor);
	}
	QTextEdit::mouseMoveEvent(e);
}

Highlighter::Highlighter(QTextDocument *parent)
	: QSyntaxHighlighter(parent)
{
	HighlightingRule rule;

	linkFormat.setForeground(Qt::blue);
	rule.format = linkFormat;
	rule.pattern = QRegExp("http://\\S+");
	highlightingRules.append(rule);
}

void Highlighter::highlightBlock(const QString &text)
{
	foreach (const HighlightingRule &rule, highlightingRules)
	{
		QRegExp expression(rule.pattern);
		int index = expression.indexIn(text);
		while (index >= 0)
		{
			int length = expression.matchedLength();
			setFormat(index, length, rule.format);
			index = expression.indexIn(text, index + length);
		}
	}
	setCurrentBlockState(0);
}
