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
#include <QCalendarWidget>
#include <QLabel>
#include <QCheckBox>
#include <QMenu>
#include <QTextStream>

enum
{
	TODO_ACTION_INSERT,
	TODO_ACTION_INSERT_SUB,
	TODO_ACTION_REMOVE,
	//TODO_ACTION_HIDE_COMPLETED,
	TODO_ACTION_COUNT,
};

TodoNote::TodoNote(const QFileInfo& fileinfo, Note::Type type_new)
	: Note(fileinfo, type_new)
{
	text_edit = new TextEdit();
	text_edit->setAcceptRichText(false);
	text_edit->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
//	text_edit->setMinimumHeight(100);
//	text_edit->setMaximumHeight(200);

	model = new TodoModel();
	proxy_model = new TodoProxyModel();
	proxy_model->setSourceModel(model);

	tree_view = new QTreeView();
	tree_view->setModel(proxy_model);
	tree_view->setEditTriggers(QAbstractItemView::DoubleClicked | QAbstractItemView::SelectedClicked);
	tree_view->setSelectionBehavior(QAbstractItemView::SelectRows);
    tree_view->header()->setSectionResizeMode(QHeaderView::ResizeToContents);
	tree_view->header()->hide();
	tree_view->setDragEnabled(true);
	tree_view->setAcceptDrops(true);
	tree_view->setDropIndicatorShown(true);
	tree_view->setDragDropMode(QAbstractItemView::InternalMove);
	tree_view->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::MinimumExpanding);

	connect(proxy_model, SIGNAL(dataChanged(const QModelIndex&, const QModelIndex&)), this, SLOT(taskChanged(QModelIndex)));
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

	lb_date_start = new QLabel;
	lb_date_0 = new QLabel(tr("Created: "), lb_date_start);
	lb_date_stop = new QLabel;
	lb_date_1 = new QLabel(tr("Completed: "), lb_date_stop);
	dt_date_limit = new QDateTimeEdit;
	dt_date_limit->setCalendarPopup(true);
	cb_date_limit = new QCheckBox(tr("Limited: "), dt_date_limit);
	cb_date_limit->setCheckable(true);

	grid_layout = new QGridLayout();
	QGridLayout* l = qobject_cast<QGridLayout*>(grid_layout);
	l->addWidget(lb_date_0, 0, 0);
	l->addWidget(lb_date_start, 0, 1);
	l->addWidget(lb_date_1, 1, 0);
	l->addWidget(lb_date_stop, 1, 1);
	l->addWidget(cb_date_limit, 2, 0);
	l->addWidget(dt_date_limit, 2, 1);

	extra_layout = new QVBoxLayout;
	extra_layout->addItem(grid_layout);
	extra_layout->addWidget(text_edit);

	extra_widget = new QWidget;
	extra_widget->setLayout(extra_layout);
	extra_widget->hide();

	main_layout = new QVBoxLayout();
	main_layout->addWidget(tree_view);
	main_layout->addWidget(extra_widget);

	area = new QScrollArea();
	area->setLayout(main_layout);

	load(); //loading note's content

	mapper = new QDataWidgetMapper();
	mapper->setModel(proxy_model);
	mapper->addMapping(text_edit, 6, "plainText");
	mapper->addMapping(lb_date_start, 2, "text");
	mapper->addMapping(lb_date_stop, 3, "text");
	lb_date_start->setLocale(settings.getLocale());
	dt_date_limit->setLocale(settings.getLocale());
	dt_date_limit->calendarWidget()->setLocale(settings.getLocale());

	tree_view->setCurrentIndex(QModelIndex());
}

TodoNote::~TodoNote()
{
	tree_view->deleteLater();
	proxy_model->deleteLater();
	model->deleteLater();
	text_edit->deleteLater();
	extra_layout->deleteLater();
	main_layout->deleteLater();
	extra_widget->deleteLater();
	area->deleteLater();
	lb_date_start->deleteLater();
	lb_date_stop->deleteLater();
	dt_date_limit->deleteLater();
	cb_date_limit->deleteLater();
	grid_layout->deleteLater();
	mapper->deleteLater();

	menu_context->deleteLater();
}

void TodoNote::retranslate(const QLocale& locale)
{
	menu_context->actions()[TODO_ACTION_INSERT]->setText(tr("Insert new task"));
	menu_context->actions()[TODO_ACTION_INSERT_SUB]->setText(tr("Insert new task"));
	menu_context->actions()[TODO_ACTION_REMOVE]->setText(tr("Remove this task"));
	//menu_context->actions()[TODO_ACTION_HIDE_COMPLETED]->setText(tr("Hide completed tasks"));
	lb_date_0->setText(tr("Created: "));
	lb_date_1->setText(tr("Completed: "));
	cb_date_limit->setText(tr("Limited: "));
	lb_date_start->setLocale(locale);
	dt_date_limit->setLocale(locale);
	dt_date_limit->calendarWidget()->setLocale(locale);
}

//Reading file
void TodoNote::load()
{
	file.close();
	if(!file.exists())
	{
		file.open(QIODevice::WriteOnly);
		file.close();
	}
	if(file.open(QIODevice::ReadOnly))
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
	//bool hide_completed = menu_context->actions()[TODO_ACTION_HIDE_COMPLETED]->isChecked();
	//proxy_model->hideDoneTasks(hide_completed);
}

void TodoNote::taskChanged(const QModelIndex& proxy_index)
{
	extra_widget->setVisible(proxy_index.isValid());
	if(!proxy_index.isValid()) return;
	mapper->setRootIndex(proxy_index.parent());
	mapper->setCurrentModelIndex(proxy_index);
	QModelIndex index = proxy_model->mapToSource(proxy_index);
	Task* task = static_cast<Task*>(index.internalPointer());
	//
	disconnect(dt_date_limit, SIGNAL(dateTimeChanged(QDateTime)), this, SLOT(noteDateLimitChanged(QDateTime)));
	disconnect(cb_date_limit, SIGNAL(toggled(bool)), this, SLOT(noteLimitChanged(bool)));
	dt_date_limit->setDateTime(task->dateLimit());
	cb_date_limit->setChecked(task->limited());
	connect(dt_date_limit, SIGNAL(dateTimeChanged(QDateTime)), this, SLOT(noteDateLimitChanged(QDateTime)));
	connect(cb_date_limit, SIGNAL(toggled(bool)), this, SLOT(noteLimitChanged(bool)));
	//
	bool task_done = task->done();
	lb_date_1->setVisible(task_done);
	lb_date_stop->setVisible(task_done);
	cb_date_limit->setHidden(task_done);
	dt_date_limit->setHidden(task_done);
	dt_date_limit->setEnabled(task->limited());

}

void TodoNote::noteDateLimitChanged(const QDateTime& date)
{
	QModelIndex proxy_index = tree_view->currentIndex();
	QModelIndex index = proxy_model->mapToSource(proxy_index);
	if(!index.isValid()) return;
	QModelIndex date_limit_index = index.sibling(index.row(), 4);
	model->setData(date_limit_index, date, Qt::EditRole);
}

void TodoNote::noteLimitChanged(bool limited)
{
	QDateTime date = limited?QDateTime::currentDateTime().addDays(7):QDateTime();
	noteDateLimitChanged(date);
}
