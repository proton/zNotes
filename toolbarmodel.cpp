#include "toolbarmodel.h"
#include "toolbaraction.h"

#include <QPalette>

//------------------------------------------------------------------------------

/*
  This model contains all existing items
*/

ItemModel::ItemModel()
{
	v.resize(itemMax);
}

int ItemModel::rowCount(const QModelIndex &) const
{
	return v.size();
}

QVariant ItemModel::data(const QModelIndex &index, int role) const
{
	const int id = index.row();
	QPalette::ColorGroup colorgroup = (isUsed(id))?QPalette::Disabled:QPalette::Normal;
	switch(role)
	{
		case Qt::DisplayRole: return ToolbarAction(item_enum(id)).text();
		case Qt::DecorationRole: return ToolbarAction(item_enum(id)).icon();
		case Qt::ForegroundRole: return QPalette().color(colorgroup, QPalette::Text);
		default: return QVariant();
	}
}

void ItemModel::setVector(const QVector<int>& nv)
{
	for(int i=0; i<v.size(); ++i) v[i]=false;
	for(int i=0; i<nv.size(); ++i)
	{
		int id = nv[i];
		if(id!=itemSeparator) v[id] = true;
	}
}

void ItemModel::insert(int id)
{
	if(id==itemSeparator) return;
	v[id] = false;
	emit reset();
}

void ItemModel::remove(int id)
{
	if(id==itemSeparator) return;
	v[id] = true;
	emit reset();
}

bool ItemModel::isUsed(int row) const
{
	return v[row];
}

//------------------------------------------------------------------------------

/*
  This model contains items, added on toolbar
*/

ItemToolbarModel::ItemToolbarModel()
{
}

int ItemToolbarModel::rowCount(const QModelIndex &) const
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

const QModelIndex& ItemToolbarModel::up(const QModelIndex &index)
{
	int row = index.row();
	if(row==0) return index; //if this item first
	qSwap(v[row], v[row-1]);
	emit reset();
	QModelIndex new_index(this->index(row-1, index.column()));
	return new_index;
}

const QModelIndex& ItemToolbarModel::down(const QModelIndex &index)
{
	int row = index.row();
	if(row==v.size()-1) return index; //if this item last
	qSwap(v[row], v[row+1]);
	emit reset();
	QModelIndex new_index(this->index(row+1, index.column()));
	return new_index;
}

void ItemToolbarModel::setVector(const QVector<int>& nv)
{
	v=nv;
}

const QVector<int>& ItemToolbarModel::getVector() const
{
	return v;
}

void ItemToolbarModel::insert(int id, int row)
{
	if(row!=-1) v.insert(row, id);
	else v.append(id);
	emit reset();
}

void ItemToolbarModel::remove(const QModelIndex & index)
{
	if(!index.isValid()) return;
	v.remove(index.row());
	emit reset();
}
