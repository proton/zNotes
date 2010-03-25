#include "todomodel.h"
#include <QStringList>
#include <QCheckBox>
#include <QtDebug>

Task::Task(QDomDocument* document, QDomNode &node, int row, Task* parent)
	: _document(document), _node(node), _row(row), _parent(parent), _done(false)
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
			else if(priority_text=="medium") _priority = medium;
			else _priority = high;
		}
		else _priority = medium;

		for(int i=0; i<node.childNodes().count(); ++i)
		{
			QDomNode child_node = _node.childNodes().item(i);
			QDomElement child_element = child_node.toElement();

			if(child_element.tagName()=="title") _title = child_element.text();
			else if(child_element.tagName()=="comment") _comment = child_element.text();

			else if(child_element.tagName()=="task") _subtasks.append(new Task(_document, child_node, _subtasks.size(), this));
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

static void updateChildNode(QDomDocument& document, QDomNode& node, const QString& tag_name, const QString& value)
{
	QDomElement old_element = node.firstChildElement(tag_name);
	QDomElement new_element = document.createElement(tag_name);
	QDomText new_element_text = document.createTextNode(value);
	new_element.appendChild(new_element_text);

	if(!old_element.isNull()) node.replaceChild(new_element, old_element);
	else node.appendChild(new_element);
}

void Task::setTitle(const QString& v)
{
	_title = v;
	updateChildNode(*_document, _node, "title", _title);
}

void Task::setComment(const QString& v)
{
	_comment = v;
	updateChildNode(*_document, _node, "comment", _comment);
}

void Task::setDateStart(const QDateTime& v)
{
	_date_start = v;
	_node.toElement().setAttribute("time_a", _date_start.toTime_t());
}

void Task::setDateStop(const QDateTime& v)
{
	_date_stop = v;
	_node.toElement().setAttribute("time_d", _date_start.toTime_t());
}

void Task::setDateLimit(const QDateTime& v)
{
	_date_limit = v;
	_node.toElement().setAttribute("time_l", _date_start.toTime_t());
}

void Task::setDone(bool v)
{
	_done = v;
	if(_done) setDateStop(QDateTime::currentDateTime());
}

void Task::setPriority(Priority v)
{
	_priority = v;
	switch(_priority)
	{
	case low: _node.toElement().setAttribute("priority", "low"); break;
	case medium: _node.toElement().setAttribute("priority", "medium"); break;
	case high: _node.toElement().setAttribute("priority", "high"); break;
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

void TodoModel::load(QDomDocument* document)
{
	//Setting document
	_document = document;

	//Removing old tasks
	delete _root_task;

	//Inserting new tasks
	QDomElement root_element = _document->documentElement();
	_root_task = new Task(_document, root_element, 0, NULL);
}

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
			case 0:
			{
				int child_count = task->subtasks().count();
				if(child_count==0) return task->title();
				int child_done = 0;
				for(int i=0; i<child_count; ++i)
					if(task->subtasks().at(i)->done())
						++child_done;
				return task->title()+QString(" (%1/%2)").arg(child_done).arg(child_count);
			}
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
	else if(role == Qt::FontRole)
	{
		if(task->done())
		{
			QFont font;
			font.setStrikeOut(true);
			return font;
		}
		return QFont();
	}
	else if(role == Qt::ForegroundRole)
	{
		QPalette::ColorGroup colorgroup = (task->done())?QPalette::Disabled:QPalette::Normal;
		return QPalette().color(colorgroup, QPalette::Text);
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
				task->setDone(data.toBool());
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
