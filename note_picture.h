#ifndef NOTE_PICTURE_H
#define NOTE_PICTURE_H

#include "note.h"

class QLabel;
class QScrollArea;

class PictureNote : public Note
{
	Q_OBJECT
public:
	PictureNote(const QFileInfo& fileinfo, Note::Type type_new);
	~PictureNote();
	void load(); //Reading note's content
	void save(bool forced = false); //Saving note's content
	void copy() const; //Coping note's content to clipboard
	QWidget* widget();
private:
	QLabel* label;
	QScrollArea* scroll_area;
};

#endif // NOTE_PICTURE_H
