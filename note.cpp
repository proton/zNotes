#include "note.h"
#include "settings.h"

#include <QTextStream>
#include <QClipboard>
#include <QApplication>

Note::Note(const QFileInfo& fileinfo)
	: file_info(fileinfo), text_edit(0), content_changed(false)
{
	type = ((file_info.suffix()=="htm")||(file_info.suffix()=="html"))?type_html:type_text;//detecting note's type
	setTitle(settings.getShowExtensions());
	load(); //loading note's content
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
		//
		if(type==type_html)
		{
			connect(text_edit, SIGNAL(currentCharFormatChanged(const QTextCharFormat &)),
				this, SLOT(currentCharFormatChanged(const QTextCharFormat &)));
		}
		break;
	default:
		break;
	}
}

Note::~Note()
{
	if(text_edit) delete text_edit;
}

//Setting note title
void Note::setTitle(bool show_extensions)
{
	file.setFileName(file_info.absoluteFilePath());
	note_title =
		(show_extensions || (file_info.fileName()[0]=='.'))?
			file_info.fileName() : file_info.baseName();
}

//Reading file
void Note::load()
{
	file.close();
	switch(type)
	{
	case type_text:
	case type_html:
	{
		TextEdit::TextType text_type = (type==type_text)?TextEdit::type_text:TextEdit::type_html;
		text_edit = new TextEdit(text_type);
		if(file.open(QIODevice::ReadOnly | QIODevice::Text))
		{
			QTextStream in(&file);
			QString text = in.readAll();
			text_edit->setText(text);
			file.close();
		}
		else if(file.open(QIODevice::WriteOnly | QIODevice::Text)) file.close(); //If file don't exist, we creating it
		break;
	}
	default: break;
	}
}

//Saving note
void Note::save(bool forced)
{
	if(!(content_changed || forced)) return; //If file doesn't need in saving, exiting from function
	file.close();
	if(!file.open(QFile::WriteOnly | QFile::Text)) return;
	QTextStream out(&file);
	switch(type)
	{
		case type_text:
 case type_html:
			out << text_edit->text();
			break;
		default:
			break;
	}
	file.close();
	content_changed = false;
}

//Renaming note
void Note::rename(const QString& new_name)
{
	file.close();
	QString absolute_file_path = file_info.dir().absoluteFilePath(new_name);
	file.rename(absolute_file_path);
	file_info.setFile(file);
	setTitle(settings.getShowExtensions());
}

//Moving note to another folder
void Note::move(const QString& new_dirname)
{
	file.close();
	QString filename = file_info.fileName();
	QString absolute_file_path = QDir(new_dirname).absoluteFilePath(filename);
	file.rename(absolute_file_path);
	file_info.setFile(file);
}

//Removing note
bool Note::remove()
{
	file.close();
	return file.remove();
}

//Returning widget (it's can be placed to tabwidget)
QWidget* Note::widget()
{
	switch(type)
	{
		case type_text:
		case type_html:
			return text_edit;
	}
}

//Coping note's content to clipboard
void Note::copy() const
{
	QClipboard* clipboard = QApplication::clipboard();
	switch(type)
	{
		case type_text:
		case type_html:
			clipboard->setText(text_edit->text());
			break;
	}
}

//Searching in a note's content
bool Note::find(const QString& text, bool next)
{
	switch(type)
	{
		case type_text:
		case type_html:
		{
			if(next) text_edit->setTextCursor(QTextCursor()); //search next
			else text_edit->unsetCursor(); //new search
			return text_edit->find(text);
		}
		default: return false;
	}
}

//Sending signal about changing format of a text under cursor
void Note::currentCharFormatChanged(const QTextCharFormat& format)
{
	emit formatChanged(format.font());
}

//Returning format of selected text
void Note::setSelFormat(const QTextCharFormat& format)
{
	if(type!=type_html) return;
	QTextCursor cursor = text_edit->textCursor();
	if(!cursor.hasSelection()) cursor.select(QTextCursor::WordUnderCursor);
	cursor.mergeCharFormat(format);
}

//Applying format to selected text
const QTextCharFormat Note::getSelFormat() const
{
	if(type!=type_html) return QTextCharFormat();
	QTextCursor cursor = text_edit->textCursor();
	return cursor.charFormat();
}

//------------------------------------------------------------------------------

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

//If notes' font changed in the preferences, applying font to note
void Note::noteFontChanged()
{
	text_edit->setFont(settings.getNoteFont());
}
