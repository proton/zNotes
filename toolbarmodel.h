#ifndef TOOLBARMODEL_H
#define TOOLBARMODEL_H

#include <QAbstractListModel>
#include <QVector>
#include <QObject>

class ToolbarItems : public QObject
{
	Q_OBJECT
public:
	ToolbarItems();
	//
	void setVector(const QVector<int>& nv);
	const QVector<int>& getToolbarElems() const { return toolbar_elems; }
	const QVector<bool>& getAllElems() const { return elems; }
	//
	void swap(int pos0, int pos1);
	void move(int pos0, int pos1);
	void insert(int id, int pos);
	void remove(int pos);
	//
	inline bool isUsed(int id) const { return elems[id]; }
signals:
	void reset();
private:
	QVector<int> toolbar_elems; // id's of elems
	QVector<bool> elems;
};

class ItemModel : public QAbstractListModel
{
public:
	ItemModel(ToolbarItems& t_items);
	int rowCount(const QModelIndex & = QModelIndex()) const;
	QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
	Qt::ItemFlags flags(const QModelIndex &index) const;
	//
	QStringList mimeTypes() const;
	QMimeData* mimeData(const QModelIndexList& indexes) const;
	bool dropMimeData(const QMimeData* data, Qt::DropAction action, int row, int column, const QModelIndex& parent);
	Qt::DropActions supportedDragActions() const;
	Qt::DropActions supportedDropActions() const;
private:
	ToolbarItems& items;
	const QVector<bool>& v;
};

class ItemToolbarModel : public QAbstractListModel
{
public:
	ItemToolbarModel(ToolbarItems& t_items);
	int rowCount(const QModelIndex& = QModelIndex()) const;
	QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;
	Qt::ItemFlags flags(const QModelIndex &index) const;
	//
	QStringList mimeTypes() const;
	QMimeData* mimeData(const QModelIndexList& indexes) const;
	bool dropMimeData(const QMimeData* data, Qt::DropAction action, int row, int column, const QModelIndex& parent);
	Qt::DropActions supportedDragActions() const;
	Qt::DropActions supportedDropActions() const;
	//
	QModelIndex up(const QModelIndex& index);
	QModelIndex down(const QModelIndex& index);
private:
	ToolbarItems& items;
	const QVector<int>& v;
};

#endif // TOOLBARMODEL_H
