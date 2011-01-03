#include "toolbarmodel.h"
#include "toolbaraction.h"

#include <QPalette>
#include <QMimeData>

#define NOTE_TOOLBAR_ITEM_MIME "application/znotes.toolbar_item"
#define NOTE_TOOLBAR_ITEM_USED_MIME "application/znotes.toolbar_item_used"

//------------------------------------------------------------------------------

ToolbarItems::ToolbarItems()
{
	elems.resize(itemMax);
}

void ToolbarItems::setVector(const QVector<int>& nv)
{
	for(int i=0; i<elems.size(); ++i) elems[i]=false;
	toolbar_elems=nv;
	for(int i=0; i<toolbar_elems.size(); ++i)
	{
		int id = toolbar_elems[i];
		if(id!=itemSeparator) elems[id] = true;
	}
}

void ToolbarItems::swap(int pos0, int pos1)
{
	qSwap(toolbar_elems[pos0], toolbar_elems[pos1]);
}

void ToolbarItems::move(int pos0, int pos1)
{
	if(pos0<0 || pos0>=toolbar_elems.size()) return;
	if(pos1<0 || pos1>toolbar_elems.size()) return;
	int id = toolbar_elems.at(pos0);
	toolbar_elems.remove(pos0);
	if(pos1<pos0) toolbar_elems.insert(pos1, id);
	else toolbar_elems.insert(pos1-1, id);
}

void ToolbarItems::insert(int id, int pos)
{
	if(pos!=-1) toolbar_elems.insert(pos, id);
	else toolbar_elems.append(id);
	if(id!=itemSeparator) elems[id] = true;
	emit reset();
}

void ToolbarItems::remove(int pos)
{
	int id = toolbar_elems.at(pos);
	toolbar_elems.remove(pos);
	elems[id] = false;
	emit reset();
}

/*
  This model contains all existing items
*/

ItemModel::ItemModel(ToolbarItems& t_items)
	: items(t_items), v(t_items.getAllElems())
{
	connect(&t_items, SIGNAL(reset()), this, SIGNAL(modelReset()));
}

int ItemModel::rowCount(const QModelIndex &) const
{
	return v.size();
}

QVariant ItemModel::data(const QModelIndex &index, int role) const
{
	const int id = index.row();
	switch(role)
	{
		case Qt::DisplayRole: return ToolbarAction(item_enum(id)).text();
		case Qt::DecorationRole: return ToolbarAction(item_enum(id)).icon();
		default: return QVariant();
	}
}

Qt::ItemFlags ItemModel::flags(const QModelIndex &index) const
{
	if(!index.isValid()) return Qt::ItemIsDropEnabled;
	if(items.isUsed(index.row())) return Qt::ItemIsDropEnabled;
	return Qt::ItemIsDragEnabled|Qt::ItemIsDropEnabled|Qt::ItemIsSelectable|Qt::ItemIsEnabled;
}

Qt::DropActions ItemModel::supportedDropActions() const
{
	return Qt::MoveAction;
}

QStringList ItemModel::mimeTypes() const
{
	QStringList types;
	types << NOTE_TOOLBAR_ITEM_USED_MIME;
	return types;
}

QMimeData* ItemModel::mimeData(const QModelIndexList& indexes) const
{
	QMimeData* mimeData = new QMimeData();
	QByteArray encodedData;
	QDataStream stream(&encodedData, QIODevice::WriteOnly);
	foreach(QModelIndex index, indexes)
	{
		stream << index.row();
	}
	mimeData->setData(NOTE_TOOLBAR_ITEM_MIME, encodedData);
	return mimeData;
}

bool ItemModel::dropMimeData(const QMimeData* data, Qt::DropAction action, int row, int column, const QModelIndex& parent)
{
	Q_UNUSED(row);
	Q_UNUSED(column);
	Q_UNUSED(parent);
	if(action==Qt::IgnoreAction) return true;
	if(!data->hasFormat(NOTE_TOOLBAR_ITEM_USED_MIME)) return false;
	if(action!=Qt::MoveAction) return true;
	//
	QByteArray encodedData = data->data(NOTE_TOOLBAR_ITEM_USED_MIME);
	QDataStream stream(&encodedData, QIODevice::ReadOnly);
	int pos;
	stream >> pos;
	items.remove(pos);
	return true;
}

//------------------------------------------------------------------------------

/*
  This model contains items, added on toolbar
*/

ItemToolbarModel::ItemToolbarModel(ToolbarItems& t_items)
	: items(t_items), v(t_items.getToolbarElems())
{
	connect(&t_items, SIGNAL(reset()), this, SIGNAL(modelReset()));
}

int ItemToolbarModel::rowCount(const QModelIndex&) const
{
	return v.size();
}

QVariant ItemToolbarModel::data(const QModelIndex &index, int role) const
{
	item_enum item = item_enum(v[index.row()]);
	switch(role)
	{
		case Qt::DisplayRole: return ToolbarAction(item).text();
		case Qt::DecorationRole : return ToolbarAction(item).icon();
		default: return QVariant();
	}
}

Qt::DropActions ItemToolbarModel::supportedDropActions() const
{
	return Qt::MoveAction;
}

Qt::ItemFlags ItemToolbarModel::flags(const QModelIndex &index) const
{
	if(!index.isValid()) return Qt::ItemIsDropEnabled;
	return Qt::ItemIsDragEnabled|Qt::ItemIsSelectable|Qt::ItemIsEnabled;
}

QStringList ItemToolbarModel::mimeTypes() const
{
	QStringList types;
	types << NOTE_TOOLBAR_ITEM_MIME << NOTE_TOOLBAR_ITEM_USED_MIME;
	return types;
}

QMimeData* ItemToolbarModel::mimeData(const QModelIndexList& indexes) const
{
	QMimeData* mimeData = new QMimeData();
	QByteArray encodedData;
	QDataStream stream(&encodedData, QIODevice::WriteOnly);
	foreach(QModelIndex index, indexes)
	{
		stream << index.row();
	}
	mimeData->setData(NOTE_TOOLBAR_ITEM_USED_MIME, encodedData);
	return mimeData;
}

bool ItemToolbarModel::dropMimeData(const QMimeData* data, Qt::DropAction action, int row, int column, const QModelIndex& parent)
{
	Q_UNUSED(column);
	Q_UNUSED(parent);
	if(row==-1) return false;
	if(action!=Qt::MoveAction) return false;
	if(data->hasFormat(NOTE_TOOLBAR_ITEM_USED_MIME))
	{
		QByteArray encodedData = data->data(NOTE_TOOLBAR_ITEM_USED_MIME);
		QDataStream stream(&encodedData, QIODevice::ReadOnly);
		int pos;
		stream >> pos;
		items.move(pos, row);
		return true;
	}
	if(data->hasFormat(NOTE_TOOLBAR_ITEM_MIME))
	{
		QByteArray encodedData = data->data(NOTE_TOOLBAR_ITEM_MIME);
		QDataStream stream(&encodedData, QIODevice::ReadOnly);
		int id;
		stream >> id;
		items.insert(id, row);
		return true;
	}
	return false;
}

QModelIndex ItemToolbarModel::up(const QModelIndex &index)
{
	int row = index.row();
	if(row==0) return index; //if this item first
	items.swap(row, row-1);
	emit reset();
	QModelIndex new_index(this->index(row-1, index.column()));
	return new_index;
}

QModelIndex ItemToolbarModel::down(const QModelIndex &index)
{
	int row = index.row();
	if(row==v.size()-1) return index; //if this item last
	items.swap(row, row+1);
	emit reset();
	QModelIndex new_index(this->index(row+1, index.column()));
	return new_index;
}
