#include <QTextStream>

#include <QRegExp>
#include <QMouseEvent>
#include <QUrl>
#include <QDesktopServices>
#include <QClipboard>
#include <QApplication>

#include <QtDebug>

#include "note.h"
#include "settings.h"

Note::Note(const QFileInfo& fileinfo)
	: file_info(fileinfo), text_edit(0), content_changed(false)
{
	type = (file_info.suffix()=="htm")?type_html:type_text;
	file.setFileName(file_info.absoluteFilePath());
	note_title = file_info.baseName();
	//
	load();
	switch(type)
	{
	case type_text:
	case type_html:
		text_edit->setMouseTracking(settings.getNoteLinksOpen());
		connect(&settings, SIGNAL(NoteLinkOpenChanged()), this, SLOT(noteLinkOpenChanged()));
		//
		text_edit->setFont(settings.getNoteFont());
		connect(&settings, SIGNAL(NoteFontChanged()), this, SLOT(noteFontChanged()));
		//
		connect(text_edit, SIGNAL(textChanged()), this, SLOT(contentChanged()));
		break;
	default:
		break;
	}
}

Note::~Note()
{
	if(text_edit) delete text_edit;
}

void Note::load()
{
	file.close();
	switch(type)
	{
	case type_text:
	case type_html:
		text_edit = new TextEdit(TextEdit::TextType(type));
		if(file.open(QIODevice::ReadOnly | QIODevice::Text))
		{
			QTextStream in(&file);
			QString text = in.readAll();
			if(type==type_text) text_edit->setPlainText(text);
			else if(type==type_html) text_edit->setHtml(text);
			file.close();
		}
		else if(file.open(QIODevice::WriteOnly | QIODevice::Text)) file.close(); //If file don't exist, we creating it
		break;
	}
}

void Note::save()
{
	if(!content_changed) return;
	file.close();
	if(!file.open(QFile::WriteOnly | QFile::Text)) return;
	QTextStream out(&file);
	switch(type)
	{
		case type_text: out << text_edit->toPlainText(); break;
		case type_html: out << text_edit->toHtml(); break;
	}
	file.close();
	content_changed = false;
}

void Note::rename(const QString& new_name)
{
	file.close();
	note_title = new_name;
	QString suffix = file_info.completeSuffix();
	QString fullname = QString("%1.%2").arg(note_title).arg(suffix);
	QString absolute_file_path = file_info.dir().absoluteFilePath(fullname);
	file.rename(absolute_file_path);
	file_info.setFile(file);
}

void Note::move(const QString& new_dirname)
{
	file.close();
	QString filename = file_info.fileName();
	QString absolute_file_path = QDir(new_dirname).absoluteFilePath(filename);
	file.rename(absolute_file_path);
	file_info.setFile(file);
}

bool Note::remove()
{
	file.close();
	return file.remove();
}

QWidget* Note::widget()
{
	switch(type)
	{
		case type_text:
		case type_html:
			return text_edit;
	}
}

void Note::copy()
{
	QClipboard* clipboard = QApplication::clipboard();
	switch(type)
	{
		case type_text: clipboard->setText(text_edit->toPlainText()); break;
		case type_html: clipboard->setText(text_edit->toHtml()); break;
	}
}

bool Note::find(const QString& text)
{
	switch(type)
	{
		case type_text:
		case type_html:
			return text_edit->find(text);
		default: return false;
	}
}

void Note::contentChanged()
{
	content_changed = true;
}

void Note::noteLinkOpenChanged()
{
	bool is_link_open = settings.getNoteLinksOpen();
	text_edit->setMouseTracking(is_link_open);
	if(!is_link_open)
	{
		text_edit->setExtraSelections(QList<QTextEdit::ExtraSelection>());
		text_edit->viewport()->setCursor(Qt::IBeamCursor);
	}
}

void Note::noteFontChanged()
{
	text_edit->setFont(settings.getNoteFont());
}

//------------------------------------------------------------------------------

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

TextEdit::TextEdit(TextType new_type)
	: QTextEdit(), type(new_type)
{
	highlighter = new Highlighter(document());
	connect(&settings, SIGNAL(NoteHighlightChanged()), highlighter, SLOT(rehighlight()));
}

void TextEdit::mousePressEvent(QMouseEvent *e)
{
	if(settings.getNoteLinksOpen() && e->buttons()==Qt::LeftButton && e->modifiers()&Qt::ControlModifier) //Ctrl+LeftMouseButton
	{
		bool onLink = false;
		int position_start, position_end;
		if(e->buttons()==Qt::NoButton && e->modifiers()&Qt::ControlModifier)
		{
			const QTextCursor cursor = cursorForPosition(e->pos());
			onLink = isOnLink(*document(), cursor, position_start, position_end);
		}
		if(onLink)
		{
			const QUrl link(toPlainText().mid(position_start, position_end));
			QDesktopServices::openUrl(link);
		}
	}
	else QTextEdit::mousePressEvent(e);
}

void TextEdit::mouseMoveEvent(QMouseEvent *e)
{
	bool onLink = false;
	int position_start, position_end;
	if(settings.getNoteLinksOpen() && e->buttons()==Qt::NoButton && e->modifiers()&Qt::ControlModifier)
	{
		const QTextCursor cursor = cursorForPosition(e->pos());
		onLink = isOnLink(*document(), cursor, position_start, position_end);
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

void TextEdit::focusOutEvent(QFocusEvent*)
{
	setExtraSelections(QList<QTextEdit::ExtraSelection>());
}
