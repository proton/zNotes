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

TodoNote::TodoNote(const QFileInfo& fileinfo, Note::Type type_new)
	: Note(fileinfo, type_new)
{
	document = new QDomDocument();

	text_edit = new TextEdit();
//	text_edit->setMouseTracking(settings.getNoteLinksOpen());
//	connect(&settings, SIGNAL(NoteLinkOpenChanged()), this, SLOT(noteLinkOpenChanged()));
//	text_edit->setFont(settings.getNoteFont());
//	connect(&settings, SIGNAL(NoteFontChanged()), this, SLOT(noteFontChanged()));
	text_edit->setAcceptRichText(false);

	model = new TodoModel();

	tree_view = new QTreeView();
	tree_view->setModel(model);
	tree_view->setEditTriggers(QAbstractItemView::DoubleClicked | QAbstractItemView::SelectedClicked);
	tree_view->setSelectionBehavior(QAbstractItemView::SelectRows);
	tree_view->header()->setResizeMode(QHeaderView::ResizeToContents);
	tree_view->header()->hide();

	connect(model, SIGNAL(dataChanged(QModelIndex,QModelIndex)), this, SLOT(taskChanged(QModelIndex)));
	connect(tree_view->selectionModel(), SIGNAL(currentChanged(QModelIndex,QModelIndex)), this, SLOT(taskChanged(QModelIndex)));

	//TODO: м.б. спросить...
	//а можно и вручную сигнал посылать при изменении
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
	if(!file.open(QIODevice::ReadOnly)) return;
	document->setContent(&file);
	model->load(document);
	file.close();
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
