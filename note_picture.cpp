#include "note_picture.h"

#include <QClipboard>
#include <QApplication>

#include <QLabel>
#include <QScrollArea>
#include <QPicture>

PictureNote::PictureNote(const QFileInfo& fileinfo, Note::Type type_new)
	: Note(fileinfo, type_new)
{
	label = new QLabel();
	label->setScaledContents(true);

	scroll_area = new QScrollArea;
	scroll_area->setWidget(label);

	load(); //loading note's content
}

PictureNote::~PictureNote()
{
	QLabel* tmp_label = label;
	QScrollArea* tmp_scroll_area = scroll_area;
	label=0;
	scroll_area=0;
	delete tmp_label;
	delete tmp_scroll_area;
}

//Reading file
void PictureNote::load()
{
	file.close();
	QImage image(file.fileName());
	label->setPixmap(QPixmap::fromImage(image));
	label->adjustSize();
}

//Saving note
void PictureNote::save(bool forced)
{
	if(!(content_changed || forced)) return; //If file doesn't need in saving, exiting from function
	file.close();
	//label->picture()->save(file.fileName()); //TODO:
	content_changed = false;
}

//Returning widget (it's can be placed to tabwidget)
QWidget* PictureNote::widget()
{
	return scroll_area;
}

//Coping note's content to clipboard
void PictureNote::copy() const
{
	QClipboard* clipboard = QApplication::clipboard();
	clipboard->setPixmap(*label->pixmap());
}
