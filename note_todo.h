#ifndef NOTE_TODO_H
#define NOTE_TODO_H

#include "note.h"

class QTreeView;
class TodoModel;
class QScrollArea;
class TextEdit;
class QDomDocument;
class QLayout;
class QDataWidgetMapper;
class QDateTimeEdit;
class QLabel;

class TodoNote : public Note
{
	Q_OBJECT
public:
	TodoNote(const QFileInfo& fileinfo, Note::Type type_new);
	~TodoNote();
	void load(); //Reading note's content
	void save(bool forced = false); //Saving note's content
	void copy() const; //Coping note's content to clipboard
	QWidget* widget();
private slots:
	void taskChanged(QModelIndex);
private:
	QDomDocument* document;
	QTreeView* tree_view;
	TodoModel* model;
	QScrollArea* area;
	TextEdit* text_edit;
	QLayout* layout;
	QLayout* hlayout;
	QDataWidgetMapper* mapper;
	QLabel* lb_date_start;
	QLabel* lb_date_stop;
	QDateTimeEdit* dt_date_limit;
	QLabel* lb_date_0;
	QLabel* lb_date_1;
	QLabel* lb_date_2;
};

#endif // NOTE_TODO_H
