#include "note_todo.h"
#include "textedit.h"
#include "todomodel.h"

#include <QClipboard>
#include <QApplication>

#include <QTreeView>
#include <QHeaderView>
#include <QScrollArea>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QDataWidgetMapper>
#include <QDateTimeEdit>
#include <QLabel>
#include <QMenu>
//#include <QAction>

class TodoView : public QTreeView
{
	//Q_OBJECT
public:
	bool isRowHidden(int row, const QModelIndex& parent) const
	{
		//qDebug() << (hide_completed && parent.child(row, 0).data(Qt::CheckStateRole).toBool());
		//return hide_completed && parent.child(row, 0).data(Qt::CheckStateRole).toBool();
		return true;
	}
	bool isIndexHidden (const QModelIndex& index) const
	{
		//qDebug() << (hide_completed && (index.sibling(index.row(), 0).data(Qt::CheckStateRole).toBool()));
		//return hide_completed && (index.sibling(index.row(), 0).data(Qt::CheckStateRole).toBool());
		return true;
	}
	inline void hideCompletedTasks(bool hide) { hide_completed = hide; }
private:
	bool hide_completed;
};

TodoNote::TodoNote(const QFileInfo& fileinfo, Note::Type type_new)
	: Note(fileinfo, type_new)
{
	//document = new QDomDocument();

	text_edit = new TextEdit();
//	text_edit->setMouseTracking(settings.getNoteLinksOpen());
//	connect(&settings, SIGNAL(NoteLinkOpenChanged()), this, SLOT(noteLinkOpenChanged()));
//	text_edit->setFont(settings.getNoteFont());
//	connect(&settings, SIGNAL(NoteFontChanged()), this, SLOT(noteFontChanged()));
	text_edit->setAcceptRichText(false);

	model = new TodoModel();

	tree_view = new TodoView();
	tree_view->setModel(model);
	tree_view->setEditTriggers(QAbstractItemView::DoubleClicked | QAbstractItemView::SelectedClicked);
	tree_view->setSelectionBehavior(QAbstractItemView::SelectRows);
	tree_view->header()->setResizeMode(QHeaderView::ResizeToContents);
	tree_view->header()->hide();

	connect(model, SIGNAL(dataChanged(QModelIndex,QModelIndex)), this, SLOT(taskChanged(QModelIndex)));
	connect(tree_view->selectionModel(), SIGNAL(currentChanged(QModelIndex,QModelIndex)), this, SLOT(taskChanged(QModelIndex)));

	tree_view->setContextMenuPolicy(Qt::CustomContextMenu);
	tree_view->setAnimated(true);

	connect(tree_view, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(contextMenuRequested(QPoint)));

	menu_context = new QMenu();
	menu_context->addAction(tr("Insert new task"), this, SLOT(insertTask()));
	menu_context->addAction(tr("Remove this task"), this, SLOT(removeTask()));
	menu_context->addAction(tr("Hide completed tasks"), this, SLOT(hideCompletedTasks()));
	menu_context->actions()[2]->setCheckable(true);

	for(int i=2; i<model->columnCount(); ++i)
		tree_view->setColumnHidden(i, true);

	lb_date_start = new QLabel();
	lb_date_0 = new QLabel(lb_date_start);
	lb_date_0->setText(tr("Started: "));
	lb_date_stop = new QLabel();
	lb_date_1 = new QLabel(lb_date_stop);
	lb_date_1->setText(tr("Stopped: "));
	dt_date_limit = new QDateTimeEdit();
	lb_date_2 = new QLabel(dt_date_limit);
	lb_date_2->setText(tr("Limited: "));

	hlayout =  new QGridLayout();
	QGridLayout* l = dynamic_cast<QGridLayout*>(hlayout);
	l->addWidget(lb_date_0, 0, 0);
	l->addWidget(lb_date_start, 0, 1);
	l->addWidget(lb_date_1, 1, 0);
	l->addWidget(lb_date_stop, 1, 1);
	l->addWidget(lb_date_2, 2, 0);
	l->addWidget(dt_date_limit, 2, 1);

	layout = new QVBoxLayout();
	layout->addWidget(tree_view);
	layout->addItem(hlayout);
	layout->addWidget(text_edit);

	area = new QScrollArea();
	area->setLayout(layout);

	load(); //loading note's content

	mapper = new QDataWidgetMapper();
	mapper->setModel(model);
	mapper->addMapping(text_edit, 6);
	mapper->addMapping(lb_date_start, 2, "text");
	mapper->addMapping(lb_date_stop, 3, "text");
	mapper->addMapping(dt_date_limit, 4);

	tree_view->setCurrentIndex(model->index(0,0));
}

TodoNote::~TodoNote()
{
	delete text_edit;
	delete area;
	delete tree_view;
	delete model;
	delete layout;
	delete hlayout;
}

//Reading file
void TodoNote::load()
{
	file.close();
	if(!file.exists() || !file.open(QIODevice::ReadOnly))
	{
		//file.open(QIODevice::WriteOnly)
	}
	else
	{
		document = model->load(file);
		file.close();
	}
}

//Saving note
void TodoNote::save(bool forced)
{
	if(!(content_changed || forced)) return; //If file doesn't need in saving, exiting from function
	file.close();
	if(file.open(QFile::WriteOnly))
	{
		QTextStream out(&file);
		document->save(out, QDomNode::EncodingFromDocument);
		content_changed = false;
	}
}

//Returning widget (it's can be placed to tabwidget)
QWidget* TodoNote::widget()
{
	return area;
}

//Coping note's content to clipboard
void TodoNote::copy() const
{
//	QClipboard* clipboard = QApplication::clipboard();
//	clipboard->setPixmap(*label->pixmap());
}

void TodoNote::contextMenuRequested(const QPoint& pos)
{
	QModelIndex index = tree_view->indexAt(pos);
	menu_context->actions()[1]->setEnabled(index.isValid()); //action "Remove this task"
	QPoint pos_global = tree_view->mapToGlobal(pos);
	menu_context->exec(pos_global);
}

void TodoNote::insertTask()
{
	QModelIndex index = tree_view->currentIndex();
	int row = model->rowCount(index)+1;
	model->insertRow(row, index);
}

void TodoNote::removeTask()
{
	QModelIndex index = tree_view->currentIndex();
	if(index.isValid())
		model->removeRow(index.row(), index.parent());
}

void TodoNote::hideCompletedTasks()
{
	bool hide_completed = menu_context->actions()[2]->isChecked();
	tree_view->hideCompletedTasks(hide_completed);
}

void TodoNote::taskChanged(QModelIndex index)
{
	Task* task = static_cast<Task*>(index.internalPointer());
	bool task_done = task->done();
	lb_date_1->setVisible(task_done);
	lb_date_stop->setVisible(task_done);
	lb_date_2->setHidden(task_done);
	dt_date_limit->setHidden(task_done);
	mapper->setCurrentModelIndex(index);
}
