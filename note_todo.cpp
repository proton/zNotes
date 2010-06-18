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

enum
{
	TODO_ACTION_INSERT,
	TODO_ACTION_INSERT_SUB,
	TODO_ACTION_REMOVE,
	TODO_ACTION_HIDE_COMPLETED,
	TODO_ACTION_COUNT,
};

TodoNote::TodoNote(const QFileInfo& fileinfo, Note::Type type_new)
	: Note(fileinfo, type_new)
{
	//document = new QDomDocument();

	text_edit = new TextEdit();
	text_edit->setAcceptRichText(false);

	model = new TodoModel();
	proxy_model = new TodoProxyModel();
	proxy_model->setSourceModel(model);

	tree_view = new QTreeView();
	tree_view->setModel(proxy_model);
	tree_view->setEditTriggers(QAbstractItemView::DoubleClicked | QAbstractItemView::SelectedClicked);
	tree_view->setSelectionBehavior(QAbstractItemView::SelectRows);
	tree_view->header()->setResizeMode(QHeaderView::ResizeToContents);
	tree_view->header()->hide();

	connect(proxy_model, SIGNAL(dataChanged(QModelIndex,QModelIndex)), this, SLOT(taskChanged(QModelIndex)));
	connect(tree_view->selectionModel(), SIGNAL(currentChanged(QModelIndex,QModelIndex)), this, SLOT(taskChanged(QModelIndex)));

	tree_view->setContextMenuPolicy(Qt::CustomContextMenu);
	tree_view->setAnimated(true);

	connect(tree_view, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(contextMenuRequested(QPoint)));

	menu_context = new QMenu();
	menu_context->addAction(tr("Insert new task"), this, SLOT(insertTask()));
	menu_context->addAction(tr("Insert new task"), this, SLOT(insertSubTask()));
	menu_context->addAction(tr("Remove this task"), this, SLOT(removeTask()));
	//menu_context->addAction(tr("Hide completed tasks"), this, SLOT(hideCompletedTasks()));
	//menu_context->actions()[TODO_ACTION_HIDE_COMPLETED]->setCheckable(true);

	for(int i=2; i<model->columnCount(); ++i)
		tree_view->setColumnHidden(i, true);

	lb_date_start = new QLabel();
	lb_date_0 = new QLabel(lb_date_start);
	lb_date_0->setText(tr("Started: "));
	lb_date_stop = new QLabel();
	lb_date_1 = new QLabel(lb_date_stop);
	lb_date_1->setText(tr("Stopped: "));
	dt_date_limit = new QDateTimeEdit();
	dt_date_limit->setCalendarPopup(true);
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
	mapper->setModel(proxy_model);
	mapper->addMapping(text_edit, 6);
	mapper->addMapping(lb_date_start, 2, "text");
	mapper->addMapping(lb_date_stop, 3, "text");
	mapper->addMapping(dt_date_limit, 4);

	tree_view->setCurrentIndex(QModelIndex());
}

TodoNote::~TodoNote()
{
	delete text_edit;
	delete area;
	delete tree_view;
	delete proxy_model;
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
	menu_context->actions()[TODO_ACTION_INSERT]->setVisible(!index.isValid());
	menu_context->actions()[TODO_ACTION_INSERT_SUB]->setVisible(index.isValid());
	menu_context->actions()[TODO_ACTION_REMOVE]->setEnabled(index.isValid());
	QPoint pos_global = tree_view->mapToGlobal(pos);
	menu_context->exec(pos_global);
}

void TodoNote::insertTask()
{
	QModelIndex index;
	int row = model->rowCount(index);
	model->insertRow(row, index);
	//Setting current index to created task
	QModelIndex child_index = model->index(row, 0);
	QModelIndex proxy_index = proxy_model->mapFromSource(child_index);
	tree_view->setCurrentIndex(proxy_index);
}

void TodoNote::insertSubTask()
{
	QModelIndex proxy_index = tree_view->currentIndex();
	QModelIndex index = proxy_model->mapToSource(proxy_index);
	int row = model->rowCount(index);
	model->insertRow(row, index);
	//Setting current index to created task
	QModelIndex child_index = index.child(row, 0);
	proxy_index = proxy_model->mapFromSource(child_index);
	tree_view->setCurrentIndex(proxy_index);
}

void TodoNote::removeTask()
{
	QModelIndex proxy_index = tree_view->currentIndex();
	QModelIndex index = proxy_model->mapToSource(proxy_index);
	if(index.isValid())
		model->removeRow(index.row(), index.parent());
}

void TodoNote::hideCompletedTasks()
{
	bool hide_completed = menu_context->actions()[TODO_ACTION_HIDE_COMPLETED]->isChecked();
	proxy_model->hideDoneTasks(hide_completed);
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
