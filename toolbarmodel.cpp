#include "toolbarmodel.h"
#include "toolbaraction.h"

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
	switch(role)
	{
		case Qt::DisplayRole: return ToolbarAction(item_enum(index.row())).text();
		case Qt::DecorationRole : return ToolbarAction(item_enum(index.row())).icon();
		default: return QVariant();
	}
}

//------------------------------------------------------------------------------

ItemToolbarModel::ItemToolbarModel()
{
}

int ItemToolbarModel::rowCount(const QModelIndex &) const
{
	return v.size();
}

QVariant ItemToolbarModel::data(const QModelIndex &index, int role) const
{
	switch(role)
	{
		case Qt::DisplayRole: return ToolbarAction(item_enum(v[index.row()])).text();
		case Qt::DecorationRole : return ToolbarAction(item_enum(v[index.row()])).icon();
		default: return QVariant();
	}
}

void ItemToolbarModel::up(const QModelIndex &index)
{
	int row = index.row();
	if(row==0) return;
	int i = v[row];
	v[row] = v[row-1];
	v[row-1] = i;
}

void ItemToolbarModel::down(const QModelIndex &index)
{
	int row = index.row();
	if(row==v.size()-1) return;
	int i = v[row];
	v[row] = v[row+1];
	v[row+1] = i;
}

void ItemToolbarModel::setVector(const QVector<int>& nv)
{
	v=nv;
}

const QVector<int>& ItemToolbarModel::getVector() const
{
	return v;
}
