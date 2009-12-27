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

void ItemModel::setVector(const QVector<int>& nv)
{
	for(int i=0; i<nv.size(); ++i)
	{
		if(nv[i]!=itemSeparator)
		{
			int id =nv[i];
			v[id] = true;
		}
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
	qSwap(v[row], v[row-1]);
	emit reset();
}

void ItemToolbarModel::down(const QModelIndex &index)
{
	int row = index.row();
	if(row==v.size()-1) return;
	qSwap(v[row], v[row+1]);
	emit reset();
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
