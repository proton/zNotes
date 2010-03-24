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
	: QAbstractItemModel(parent), _root_task(0)
{
}

TodoModel::~TodoModel()
{
	delete _root_task;
}

void TodoModel::load(const QDomDocument& document)
{
	//Setting document
	_document = document;

	//Removing old tasks
	delete _root_task;

	//Inserting new tasks
	QDomElement root_element = _document.documentElement();
	_root_task = new Task(root_element, 0, NULL);
}

//void TodoModel::save()
//{
//	//QDomElement root_element = _document.documentElement();
//	//_document.save();
//}

int TodoModel::columnCount(const QModelIndex& parent) const
{
	Q_UNUSED(parent)
	return 7;
}

QString getDateGap(const QDateTime& dt)
{
	QDateTime current(QDateTime::currentDateTime());

	int days_gap = dt.daysTo(current);
	if(days_gap) return QObject::tr("%n days(s)", "", days_gap);

	int secs_gap = dt.secsTo(current);
	if(secs_gap/3600) return QObject::tr("%n hour(s)", "", secs_gap/3600);
	if(secs_gap/60) return QObject::tr("%n min(s)", "", secs_gap/60);
	else return QObject::tr("%n sec(s)", "", secs_gap);
}

QVariant TodoModel::data(const QModelIndex& index, int role) const
{
	if(!index.isValid()) return QVariant();

	Task* task = static_cast<Task*>(index.internalPointer());

//	if(index.column()==4)
//	{
//		qDebug() << role << task->dateLimit();
//	}

	if(role == Qt::DisplayRole)
	{
		switch(index.column())
		{
			case 0: return task->title();
			case 1: return (!task->done())?getDateGap(task->dateLimit()):"";
			case 2: return task->dateStart();
			case 3: return task->dateStop();
			case 4: return task->dateLimit();
			case 5: return task->priority();
			case 6: return task->comment();
			default: return QVariant();
		}
	}
	else if(role == Qt::EditRole)
	{
		switch(index.column())
		{
			case 0: return task->title();
			case 1: if(!task->done()) task->dateLimit();
			case 2: return task->dateStart().toString();
			case 3: return task->dateStop().toString();
			case 4: return task->dateLimit();
			case 6: return task->comment();
			default: return QVariant();
		}
	}
	else if(role == Qt::CheckStateRole)
	{
		switch(index.column())
		{
			case 0: return (task->done())?Qt::Checked:Qt::Unchecked;
			default: return QVariant();
		}
	}
	return QVariant();
}

bool TodoModel::setData(const QModelIndex& index, const QVariant& data, int role)
{
	Task* task = static_cast<Task*>(index.internalPointer());
	switch(index.column())
	{
		case 0:
			if(role == Qt::CheckStateRole)
			{
				bool task_done = data.toBool();
				task->setDone(task_done);
				if(task_done) task->setDateStop(QDateTime::currentDateTime());
				return true;
			}
			else if(role == Qt::EditRole)
			{
				task->setTitle(data.toString());
				return true;
			}
//		case 2:
//			if(role == Qt::EditRole)
//			{
//				task->setDateStart(data.toDateTime());
//				return true;
//			}
//		case 3:
//			if(role == Qt::EditRole)
//			{
//				task->setDateStop(data.toDateTime());
//				return true;
//			}
		case 4:
			if(role == Qt::EditRole)
			{
				task->setDateLimit(data.toDateTime());
				return true;
			}
		case 6:
			if(role == Qt::EditRole)
			{
				task->setComment(data.toString());
				return true;
			}
		default:
		return QAbstractItemModel::setData(index, data, role);
	}
	return QAbstractItemModel::setData(index, data, role);
}

Qt::ItemFlags TodoModel::flags(const QModelIndex& index) const
{
	switch(index.column())
	{
		case 0: return Qt::ItemIsSelectable|Qt::ItemIsEnabled|Qt::ItemIsUserCheckable|Qt::ItemIsEditable;
		case 1: return Qt::ItemIsSelectable|Qt::ItemIsEnabled;
		//case 2: return Qt::ItemIsSelectable|Qt::ItemIsEnabled|Qt::ItemIsEditable;
		default: return 0;
	}
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
	//if(!hasIndex(row, column, parent)) return QModelIndex();

	Task* parent_item;

	 if (!parent.isValid()) parent_item = _root_task;
	 else parent_item = static_cast<Task*>(parent.internalPointer());

	 Task* child_item = parent_item->subtasks().at(row);
	 if(child_item) return createIndex(row, column, child_item);
	 else return QModelIndex();
}

QModelIndex TodoModel::parent(const QModelIndex& child) const
{
	if (!child.isValid()) return QModelIndex();

	Task* child_item = static_cast<Task*>(child.internalPointer());
	Task* parent_item = child_item->parent();

	if(parent_item==_root_task) return QModelIndex();

	return createIndex(parent_item->row(), 0, parent_item);
}

int TodoModel::rowCount(const QModelIndex& parent) const
{
	Task* parent_item;
	if(parent.column() > 0) return 0;
	if(!parent.isValid()) parent_item = _root_task;
	else parent_item = static_cast<Task*>(parent.internalPointer());
	return parent_item->subtasks().size();
}
