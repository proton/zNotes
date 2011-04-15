#ifndef NOTECREATEBUTTON_H
#define NOTECREATEBUTTON_H

#include <QPushButton>
#include "notetype.h"

class NoteList;

class NoteCreateButton : public QPushButton
{
	Q_OBJECT
public:
	explicit NoteCreateButton(QWidget *parent, const NoteType& ntype);
	//void paintEvent(QPaintEvent* e);

signals:

public slots:

private:
	const NoteType& note_type;
	NoteList* note_list;


};

#endif // NOTECREATEBUTTON_H
