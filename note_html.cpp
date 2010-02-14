#include "note_html.h"

#include <QTextStream>
#include <QClipboard>
#include <QApplication>

HtmlNote::HtmlNote(const QFileInfo& fileinfo, Note::Type type_new)
	: Note(fileinfo, type_new)
{
	load(); //loading note's content

	text_edit.setMouseTracking(settings.getNoteLinksOpen());
	connect(&settings, SIGNAL(NoteLinkOpenChanged()), this, SLOT(noteLinkOpenChanged()));

	text_edit.setFont(settings.getNoteFont());
	connect(&settings, SIGNAL(NoteFontChanged()), this, SLOT(noteFontChanged()));

	connect(&text_edit, SIGNAL(textChanged()), this, SLOT(contentChanged()));

	connect(&text_edit, SIGNAL(currentCharFormatChanged(const QTextCharFormat &)),
		this, SLOT(currentCharFormatChanged(const QTextCharFormat &)));

	text_edit.setAcceptRichText(true);
}

//Reading file
void HtmlNote::load()
{
	file.close();
	if(file.open(QIODevice::ReadOnly | QIODevice::Text))
	{
		QTextStream in(&file);
		QString text = in.readAll();
		text_edit.setHtml(text);
		file.close();
	}
	else if(file.open(QIODevice::WriteOnly | QIODevice::Text)) file.close(); //If file don't exist, we creating it
}

//Saving note
void HtmlNote::save(bool forced)
{
	if(!(content_changed || forced)) return; //If file doesn't need in saving, exiting from function
	file.close();
	if(!file.open(QFile::WriteOnly | QFile::Text)) return;
	QTextStream out(&file);
	out << text_edit.toHtml();
	file.close();
	content_changed = false;
}

//Returning widget (it's can be placed to tabwidget)
QWidget* HtmlNote::widget()
{
	return &text_edit;
}

//Coping note's content to clipboard
void HtmlNote::copy() const
{
	QClipboard* clipboard = QApplication::clipboard();
	clipboard->setText(text_edit.toHtml());
}

//Searching in a note's content
bool HtmlNote::find(const QString& text, bool next)
{
	if(next) text_edit.setTextCursor(QTextCursor()); //search next
	else text_edit.unsetCursor(); //new search
	return text_edit.find(text);
}

void HtmlNote::noteLinkOpenChanged()
{
	bool is_link_open = settings.getNoteLinksOpen();
	text_edit.setMouseTracking(is_link_open);
	if(!is_link_open)
	{
		text_edit.setExtraSelections(QList<QTextEdit::ExtraSelection>());
		text_edit.viewport()->setCursor(Qt::IBeamCursor);
	}
}

//If notes' font changed in the preferences, applying font to note
void HtmlNote::noteFontChanged()
{
	text_edit.setFont(settings.getNoteFont());
}

//Sending signal about changing format of a text under cursor
void HtmlNote::currentCharFormatChanged(const QTextCharFormat& format)
{
	emit formatChanged(format.font());
}

//Applying format to selected text
void HtmlNote::setSelFormat(const QTextCharFormat& format)
{
	QTextCursor cursor = text_edit.textCursor();
	if(!cursor.hasSelection()) cursor.select(QTextCursor::WordUnderCursor);
	cursor.mergeCharFormat(format);
}

//Returning format of selected text
QTextCharFormat HtmlNote::getSelFormat() const
{
	QTextCursor cursor = text_edit.textCursor();
	return cursor.charFormat();
}

