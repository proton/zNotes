#ifndef HTMLNOTE_H
#define HTMLNOTE_H

#include "note.h"

class TextEdit;

class HtmlNote : public Note
{
	Q_OBJECT
public:
	HtmlNote(const QFileInfo& fileinfo, Note::Type type_new);
	~HtmlNote();
	void load(); //Reading note's content
	void save(bool forced = false); //Saving note's content
	void copy() const; //Coping note's content to clipboard
	bool find(const QString& text, bool next=false); //Searching text in a note's content
	QWidget* widget();
	void setSelFormat(const QTextCharFormat& format);
	QTextCharFormat getSelFormat() const;
    bool isDocumentSupported() const;
    QTextDocument* document() const;

private slots:
	void currentCharFormatChanged(const QTextCharFormat&);
	void noteNotePastePlaintextChanged();

private:
	TextEdit* text_edit;

signals:
	void formatChanged(const QFont& font);
};

#endif // HTMLNOTE_H
