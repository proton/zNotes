#ifndef TODONOTE_H
#define TODONOTE_H

#include "note.h"

class QTreeView;
class TodoModel;
class TodoProxyModel;
class QScrollArea;
class TextEdit;
class QDomDocument;
class QLayout;
class QDataWidgetMapper;
class QDateTimeEdit;
class QLabel;
class QCheckBox;
class QMenu;

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
	void retranslate(const QLocale& locale);
    bool isDocumentSupported() const;
    QTextDocument* document() const;

private slots:
	void taskChanged(const QModelIndex& proxy_index);
	void contextMenuRequested(const QPoint& pos);
	void insertTask();
	void insertSubTask();
	void removeTask();
	void hideCompletedTasks();
	//bool QAbstractItemView::isIndexHidden ( const QModelIndex & index ) const;
	void noteDateLimitChanged(const QDateTime&);
	void noteLimitChanged(bool);

private:
    QDomDocument* domDocument;
	QTreeView* tree_view;
	TodoModel* model;
	TodoProxyModel* proxy_model;
	QScrollArea* area;
	TextEdit* text_edit;
	QWidget* extra_widget;
	QLayout* main_layout;
	QLayout* grid_layout;
	QLayout* extra_layout;
	QDataWidgetMapper* mapper;
	QLabel* lb_date_start;
	QLabel* lb_date_stop;
	QDateTimeEdit* dt_date_limit;
	QLabel* lb_date_0;
	QLabel* lb_date_1;
	QCheckBox* cb_date_limit;
	QMenu* menu_context;
};

#endif // TODONOTE_H
