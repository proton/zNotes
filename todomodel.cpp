#include "todomodel.h"
#include <QStringList>
#include <QCheckBox>
#include <QtDebug>

Task::Task(QDomNode &node, int row, Task* parent)
	: _node(node), _row(row), _parent(parent), _done(false)
{
	QDomElement element = _node.toElement();
	if(!element.isNull())
	{
		if(element.attributes().contains("time_a")) _date_start = QDateTime::fromTime_t(element.attribute("time_a").toInt());
		if(element.attributes().contains("time_l")) _date_limit = QDateTime::fromTime_t(element.attribute("time_l").toInt());
		if(element.attributes().contains("time_d"))
		{
			_date_stop = QDateTime::fromTime_t(element.attribute("time_d").toInt());
			_done = true;
		}
		if(element.attributes().contains("priority"))
		{
			const QString priority_text = element.attribute("priority");
			if(priority_text=="low") _priority = low;
			else if(priority_text=="normal") _priority = normal;
			else _priority = high;
		}
		else _priority = normal;

		for(int i=0; i<node.childNodes().count(); ++i)
		{
			QDomNode child_node = _node.childNodes().item(i);
			QDomElement child_element = child_node.toElement();

			if(child_element.tagName()=="title") _title = child_element.text();
			else if(child_element.tagName()=="comment") _comment = child_element.text();

			else if(child_element.tagName()=="task") _subtasks.append(new Task(child_node, _subtasks.size(), this));
		}
	}
}

Task::~Task()
{
	for(int i=0; i<_subtasks.size(); ++i)
	{
		delete _subtasks[i];
		_subtasks[i] = 0;
	}
}

//------------------------------------------------------------------------------

TodoModel::TodoModel(QObject *parent)
	: QAbstractItemModel(parent)
{
}

TodoModel::~TodoModel()
{
	for(int i=0; i<_tasks.size(); ++i)
	{
		delete _tasks[i];
		_tasks[i] = 0;
	}
}

void TodoModel::load(const QDomDocument& document)
{
	//Setting document
	_document = document;

	//Removing old tasks
	for(int i=0; i<_tasks.size(); ++i)
	{
		delete _tasks[i];
		_tasks[i] = 0;
	}
	_tasks.clear();

	//Inserting new tasks
	QDomElement root_element = _document.documentElement();
	QDomNode node = root_element.firstChild();
	while(!node.isNull())
	{
		QDomElement element = node.toElement();
		if(!element.isNull() && element.tagName()=="task")
		{
			_tasks.append(new Task(node, _tasks.size(), NULL));
		}
		node = node.nextSibling();
	}

}

int TodoModel::columnCount(const QModelIndex& parent) const
{
	Q_UNUSED(parent)
	return 3;
}

QString getDateGap(const QDateTime& dt)
{
	QDateTime current(QDateTime::currentDateTime());

	int days_gap = dt.daysTo(current);
	if(days_gap) return QObject::tr("%n days(s)", "", days_gap);

	int secs_gap = dt.secsTo(current);
	if(secs_gap/3600) return QObject::tr("%n hour(s)", "", days_gap);
	if(secs_gap/60) return QObject::tr("%n min(s)", "", days_gap);
	else return QObject::tr("%n sec(s)", "", days_gap);
}

QVariant TodoModel::data(const QModelIndex& index, int role) const
{
	if(!index.isValid()) return QVariant();
	if(role != Qt::DisplayRole) return QVariant();

	Task* task = static_cast<Task*>(index.internalPointer());

	switch(index.column())
	{
	case 0: return task->done();
	case 1: return task->title();
	case 2: if(!task->done()) return getDateGap(task->dateLimit());
	default: return QVariant();
	}
	return QVariant();
}

Qt::ItemFlags TodoModel::flags(const QModelIndex& index) const
{
	if (!index.isValid()) return 0;
	return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

QVariant TodoModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	Q_UNUSED(section)
	Q_UNUSED(orientation)
	Q_UNUSED(role)
//	if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
//	{
//		switch (section)
//		{
//			case 0: return tr("Name");
//			case 1: return tr("Attributes");
//			case 2: return tr("Value");
//			default: return QVariant();
//		}
//	}
	return QVariant();
}

QModelIndex TodoModel::index(int row, int column, const QModelIndex& parent) const
{
	if(!hasIndex(row, column, parent)) return QModelIndex();

	Task* parent_item;
	Task* child_item;

	if(!parent.isValid()) child_item = _tasks.at(row);
	else
	{
		parent_item = static_cast<Task*>(parent.internalPointer());
		child_item = parent_item->subtasks().at(row);
	}

	if(child_item) return createIndex(row, column, child_item);
	else return QModelIndex();
}

QModelIndex TodoModel::parent(const QModelIndex& child) const
{
	if (!child.isValid()) return QModelIndex();

	Task* child_item = static_cast<Task*>(child.internalPointer());
	Task* parent_item = child_item->parent();

	if (parent_item) createIndex(parent_item->row(), 0, parent_item);
	return QModelIndex();
}

int TodoModel::rowCount(const QModelIndex& parent) const
{
	if(parent.column() > 0) return 0;
	if (!parent.isValid()) return _tasks.size();
	Task* parent_item = static_cast<Task*>(parent.internalPointer());
	return parent_item->subtasks().size();
}

//------------------------------------------------------------------------------

CheckBoxDelegate::CheckBoxDelegate(QObject *parent)
	: QItemDelegate(parent)
{
}

QWidget* CheckBoxDelegate::createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
	Q_UNUSED(option)
	Q_UNUSED(index)
	qDebug() << __LINE__;
	QCheckBox* editor = new QCheckBox(parent);
	return editor;
}

void CheckBoxDelegate::setEditorData(QWidget* editor, const QModelIndex &index) const
{
	qDebug() << __LINE__;
	bool value = index.model()->data(index, Qt::EditRole).toBool();
	QCheckBox* checkBox = static_cast<QCheckBox*>(editor);
	checkBox->setChecked(value);
}

void CheckBoxDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
	qDebug() << __LINE__;
	QCheckBox* checkBox = static_cast<QCheckBox*>(editor);
	bool value = checkBox->isChecked();

	model->setData(index, value, Qt::EditRole);
}

void CheckBoxDelegate::updateEditorGeometry(QWidget *editor,
	const QStyleOptionViewItem &option, const QModelIndex &/* index */) const
{
	editor->setGeometry(option.rect);
}
