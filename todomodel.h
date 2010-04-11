#ifndef TODOMODEL_H
#define TODOMODEL_H

#include <QDomNode>
#include <QHash>

#include <QAbstractItemModel>
#include <QDomDocument>
#include <QModelIndex>
#include <QDateTime>
#include <QVariant>
#include <QFile>

#include <QItemDelegate>

class Task
{
public:
	Task(QDomDocument* document, QDomNode &node, int row, Task *parent = 0);
	~Task();

	enum Priority
	{
		medium, low, high
	};

	inline const QList<Task*>& subtasks() const { return _subtasks; }
	inline Task* parent() const { return _parent; }
	inline int row() const { return _row; };

	inline const QDomNode& node() const { return _node; }

	inline const QString& title() const { return _title; }
	inline const QString& comment() const { return _comment; }
	inline const QDateTime& dateStart() const { return _date_start; }
	inline const QDateTime& dateStop() const { return _date_stop; }
	inline const QDateTime& dateLimit() const { return _date_limit; }
	inline bool done() const { return _done; }
	inline Priority priority() const { return _priority; }

	void setTitle(const QString& v);
	void setComment(const QString& v);
	void setDateStart(const QDateTime& v);
	void setDateStop(const QDateTime& v);
	void setDateLimit(const QDateTime& v);
	void setDone(bool v);
	void setPriority(Priority v);

	void insertSubTask();
	void removeSubTask(int pos);

private:
	QDomDocument* _document;

	QDomNode _node;
	int _row;
	Task* _parent;
	QList<Task*> _subtasks;

	QString _title;
	QString _comment;
	QDateTime _date_start, _date_stop, _date_limit;
	Priority _priority;
	bool _done;
};

class TodoModel : public QAbstractItemModel
{
	Q_OBJECT

public:
	TodoModel(QObject *parent = 0);
	~TodoModel();

	QVariant data(const QModelIndex& index, int role) const;
	bool setData(const QModelIndex& index, const QVariant& data, int role);
	Qt::ItemFlags flags(const QModelIndex &index) const;
	QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
	QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const;
	QModelIndex parent(const QModelIndex &child) const;
	int rowCount(const QModelIndex &parent = QModelIndex()) const;
	int columnCount(const QModelIndex &parent = QModelIndex()) const;
	bool hasChildren(const QModelIndex & parent = QModelIndex()) const;

	QDomDocument* load(QFile& file);

	bool insertRows(int row, int count, const QModelIndex& parent = QModelIndex());
	bool removeRows(int row, int count, const QModelIndex& parent = QModelIndex());

private:
	QDomDocument* _document;
	Task* _root_task;
};

#endif // TODOMODEL_H
