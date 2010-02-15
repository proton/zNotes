#include "note_text.h"
#include "textedit.h"

#include <QTextStream>
#include <QClipboard>
#include <QApplication>

TextNote::TextNote(const QFileInfo& fileinfo, Note::Type type_new)
	: Note(fileinfo, type_new)
{
	text_edit = new TextEdit();

	load(); //loading note's content

	text_edit->setMouseTracking(settings.getNoteLinksOpen());
	connect(&settings, SIGNAL(NoteLinkOpenChanged()), this, SLOT(noteLinkOpenChanged()));

	text_edit->setFont(settings.getNoteFont());
	connect(&settings, SIGNAL(NoteFontChanged()), this, SLOT(noteFontChanged()));

	connect(text_edit, SIGNAL(textChanged()), this, SLOT(contentChanged()));

	text_edit->setAcceptRichText(false);
}

TextNote::~TextNote()
{
	delete text_edit;
}

//Reading file
void TextNote::load()
{
	file.close();
	if(file.open(QIODevice::ReadOnly | QIODevice::Text))
	{
		QTextStream in(&file);
		QString text = in.readAll();
		text_edit->setPlainText(text);
		file.close();
	}
	else if(file.open(QIODevice::WriteOnly | QIODevice::Text)) file.close(); //If file don't exist, we creating it
}

//Saving note
void TextNote::save(bool forced)
{
	if(!(content_changed || forced)) return; //If file doesn't need in saving, exiting from function
	file.close();
	if(!file.open(QFile::WriteOnly | QFile::Text)) return;
	QTextStream out(&file);
	out << text_edit->toPlainText();
	file.close();
	content_changed = false;
}

//Returning widget (it's can be placed to tabwidget)
QWidget* TextNote::widget()
{
	return text_edit;
}

//Coping note's content to clipboard
void TextNote::copy() const
{
	QClipboard* clipboard = QApplication::clipboard();
	clipboard->setText(text_edit->toPlainText());
}

//Searching in a note's content
bool TextNote::find(const QString& text, bool next)
{
	if(next) text_edit->setTextCursor(QTextCursor()); //search next
	else text_edit->unsetCursor(); //new search
	return text_edit->find(text);
}

void TextNote::noteLinkOpenChanged()
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
void TextNote::noteFontChanged()
{
	text_edit->setFont(settings.getNoteFont());
}

