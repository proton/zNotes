#include "todomodel.h"
#include <QStringList>
#include <QCheckBox>
#include <QMimeData>

#include <QtDebug>

#define NOTE_TODO_TASK_MIME "application/znotes.content.list"

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

void Task::insertSubTask(int pos)
{
	QDomElement new_task_elem = _document->createElement("task");
	if(pos<0 || pos>_subtasks.size()) pos = _subtasks.size();
	if(pos == _subtasks.size()) _node.appendChild(new_task_elem);
	else _node.insertAfter(new_task_elem, _node.childNodes().at(pos-1));
	Task* task = new Task(_document, new_task_elem, pos, this);
	task->setDateStart(QDateTime::currentDateTime());
	task->setTitle(QObject::tr("New task"));
	_subtasks.insert(pos, task);
}

void Task::removeSubTask(int pos)
{
	Task* subtask = _subtasks.takeAt(pos);

	const QDomNode& subtask_elem = subtask->node();
	_node.removeChild(subtask_elem);

	Task* subtask_new = subtask;
	subtask = 0;
	delete subtask_new;
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
	_node.toElement().setAttribute("time_d", _date_stop.toTime_t());
}

void Task::setDateLimit(const QDateTime& v)
{
	_date_limit = v;
	_node.toElement().setAttribute("time_l", _date_limit.toTime_t());
}

void Task::setDone(bool v)
{
	_done = v;
	if(_done)
	{
		setDateStop(QDateTime::currentDateTime());
//		for(int i=0; i<_subtasks.size(); ++i)
//		{
//			Task* subtask = _subtasks[i];
//			if(!subtask->done()) subtask->setDone(true);
//		}
	}
	else _node.toElement().removeAttribute("time_d");
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
	: QAbstractItemModel(parent), _document(0), _root_task(0)
{
}

TodoModel::~TodoModel()
{
	delete _root_task;
}

QDomDocument*  TodoModel::load(QFile& file)
{
	delete _document;
	_document = new QDomDocument("ztodo");

	//Creating new TODO note
	if(!_document->setContent(&file) || !_document->childNodes().count())
	{
		 _document->appendChild(_document->createProcessingInstruction("xml", "version='1.0' encoding='UTF-8'"));

		QDomElement elem = _document->createElement("ztodo");
		elem.setAttribute("version", VERSION);
		_document->appendChild(elem);
	}

	//Removing old tasks
	delete _root_task;

	//Inserting new tasks
	QDomElement root_element = _document->documentElement();
	_root_task = new Task(_document, root_element, 0, NULL);

	reset();

	return _document;
}

bool TodoModel::insertRows(int row, int count, const QModelIndex& parent)
{
	Task* task = getTask(parent);
	beginInsertRows(parent, row, row+count-1);
	for(int i=0; i<count; ++i)
	{
		task->insertSubTask(row+i);
	}
	endInsertRows();
	return true;
}

bool TodoModel::removeRows(int row, int count, const QModelIndex& parent)
{
	Task* task_parent = getTask(parent);
	beginRemoveRows(parent, row, row+count-1);
	for(int i=0; i<count; ++i)
	{
		task_parent->removeSubTask(row+i);
	}
	endRemoveRows();
	return true;
}

Qt::DropActions TodoModel::supportedDropActions() const
{
	return Qt::MoveAction;
}

QStringList TodoModel::mimeTypes() const
{
	QStringList types;
	types << NOTE_TODO_TASK_MIME;
	return types;
}

QMimeData* TodoModel::mimeData(const QModelIndexList& indexes) const
{
	QMimeData* mimeData = new QMimeData();
	QByteArray encodedData;
	QDataStream stream(&encodedData, QIODevice::WriteOnly);
	foreach(QModelIndex index, indexes)
	{
		Task* task = getTask(index);
		stream << task->title();
		stream << task->comment();
		stream << task->dateStart().toTime_t();
		stream << task->dateStop().toTime_t();
		stream << task->dateLimit().toTime_t();
		stream << task->done();
		stream << int(task->priority());
//		if (index.isValid())
//		{
//			QString text = data(index, Qt::DisplayRole).toString();
//			stream << text;
//		}
	}
	mimeData->setData(NOTE_TODO_TASK_MIME, encodedData);
	return mimeData;
}

bool TodoModel::dropMimeData(const QMimeData* data, Qt::DropAction action, int row, int column, const QModelIndex& parent)
{
	Q_UNUSED(column);
	if(action==Qt::IgnoreAction) return true;
	if(!parent.isValid()) return false;
	if(!data->hasFormat(NOTE_TODO_TASK_MIME)) return false;
	if(action!=Qt::MoveAction) return true;
	Task* task_parent = getTask(parent);
	const QList<Task*>& task_list = task_parent->subtasks();
	if(row==-1) row = task_list.size();
	if(insertRow(row, parent))
	{
		Task* task = task_list.at(row);
		//
		QByteArray encodedData = data->data(NOTE_TODO_TASK_MIME);
		QDataStream stream(&encodedData, QIODevice::ReadOnly);
		QString title, comment;
		uint date_start_ts, date_stop_ts, date_limit_ts;
		bool done;
		int priority;
		//
		stream >> title >> comment
				>> date_start_ts >> date_stop_ts >> date_limit_ts
				>> done >> priority;
		task->setTitle(title);
		task->setComment(comment);
		task->setDateStart(QDateTime::fromTime_t(date_start_ts));
		if(done) task->setDateStop(QDateTime::fromTime_t(date_stop_ts));
		if(date_limit_ts) task->setDateLimit(QDateTime::fromTime_t(date_limit_ts));
		task->setPriority(Task::Priority(priority));
		return true;
	}
	return false;
}

int TodoModel::rowCount(const QModelIndex& index) const
{
	//if(!parent.isValid()) return 0;
	if(index.column() > 0) return 0;
	Task* task = getTask(index);
	if(!task) return 0;
	return task->subtasks().size();
}

int TodoModel::columnCount(const QModelIndex& parent) const
{
	Q_UNUSED(parent)
	return 7;
}

Task* TodoModel::getTask(const QModelIndex &index) const
{
	if(index.isValid())
	{
		Task* task = static_cast<Task*>(index.internalPointer());
		if(task) return task;
	}
	return _root_task;
}

QString getDateGap(const QDateTime& dest_date)
{
	if(dest_date.isNull()) return QString();

	QDateTime current = QDateTime::currentDateTime();

	int days_gap = current.daysTo(dest_date);
	int secs_gap = current.secsTo(dest_date);

	if(secs_gap<0) return QObject::tr("expired");

	if(days_gap) return QObject::tr("%n day(s)", "", days_gap);
	if(secs_gap>=3600) return QObject::tr("%n hour(s)", "", secs_gap/3600);
	if(secs_gap>=60) return QObject::tr("%n minute(s)", "", secs_gap/60);
	return QObject::tr("%n second(s)", "", secs_gap);
}

QVariant TodoModel::data(const QModelIndex& index, int role) const
{
	if(!index.isValid()) return QVariant();

	Task* task = static_cast<Task*>(index.internalPointer());

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
				emit dataChanged(index, index);
				return true;
			}
			else if(role == Qt::EditRole)
			{
				task->setTitle(data.toString());
				emit dataChanged(index, index);
				return true;
			}
		case 4:
			if(role == Qt::EditRole)
			{
				QDateTime date = data.toDateTime();
				task->setDateLimit(date);
				emit dataChanged(index, index);
				QModelIndex date_display_index = index.sibling(index.row(), 1);
				emit dataChanged(date_display_index, date_display_index);
				return true;
			}
		case 6:
			if(role == Qt::EditRole)
			{
				task->setComment(data.toString());
				emit dataChanged(index, index);
				return true;
			}
		default:
		return QAbstractItemModel::setData(index, data, role);
	}
	return QAbstractItemModel::setData(index, data, role);
}

Qt::ItemFlags TodoModel::flags(const QModelIndex& index) const
{
	if(!index.isValid()) return Qt::ItemIsDropEnabled;
	switch(index.column())
	{
		case 0: return  Qt::ItemIsDragEnabled|Qt::ItemIsDropEnabled|Qt::ItemIsSelectable|Qt::ItemIsEnabled|Qt::ItemIsUserCheckable|Qt::ItemIsEditable;
		case 1: return  Qt::ItemIsDragEnabled|Qt::ItemIsDropEnabled|Qt::ItemIsSelectable|Qt::ItemIsEnabled;
		default: return Qt::ItemIsDropEnabled|Qt::ItemIsDropEnabled|Qt::ItemIsSelectable|Qt::ItemIsEnabled;
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

	Task* parent_item = getTask(parent);

	Task* child_item = (parent_item->subtasks().size()>row)?
		parent_item->subtasks().at(row):0;

	if(child_item) return createIndex(row, column, child_item);
	else return QModelIndex();
}

QModelIndex TodoModel::parent(const QModelIndex& child) const
{
	if (!child.isValid()) return QModelIndex();

	Task* child_item = getTask(child);
	Task* parent_item = child_item->parent();

	//if(parent_item==_root_task) return index(0,0);
	if(parent_item==_root_task) return QModelIndex();

	return createIndex(parent_item->row(), 0, parent_item);
}

TodoProxyModel::TodoProxyModel()
	: QSortFilterProxyModel(), hide_done_tasks(false)
{
}


void TodoProxyModel::hideDoneTasks(bool hide)
{
	hide_done_tasks = hide;
	emit filterChanged();
}

bool TodoProxyModel::filterAcceptsRow (int source_row, const QModelIndex& source_parent) const
{
	if(hide_done_tasks)
	{
//		qDebug() << __LINE__;
		//тут херятся первые строки =(
		QModelIndex source_index = source_parent.isValid() ?
			source_parent.child(source_row, 0) :
			sourceModel()->index(0,0).child(source_row, 0);
//		qDebug() << __LINE__;
		//
//		qDebug() << source_index.isValid() << source_index.data(Qt::CheckStateRole).toBool() << source_index.data().toString();
		//
		if(source_index.isValid())
		{
//			qDebug() << __LINE__;
			bool done = source_index.data(Qt::CheckStateRole).toBool();
//			qDebug() << __LINE__;
			if(done) return false;
		}
	}
	return true;
}
