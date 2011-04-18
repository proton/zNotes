#include "highlightrulemodel.h"

HighlightRuleModel::HighlightRuleModel(QObject* parent, const QVector<HighlightRule>& r) :
	QAbstractItemModel(parent), rules(r)
{
}

int	HighlightRuleModel::columnCount(const QModelIndex&) const
{
	return 2;
}

int	HighlightRuleModel::rowCount(const QModelIndex&) const
{
	return rules.size();
}

QVariant HighlightRuleModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if(role!=Qt::DisplayRole) return QVariant();
	switch(orientation)
	{
		case Qt::Horizontal:
			switch(section)
			{
				case 0: return tr("Pattern");
				case 1: return tr("Color");
			}
		case Qt::Vertical: return (section+1);
	}
	return QVariant();
}

QVariant HighlightRuleModel::data(const QModelIndex& index, int role) const
{
	if(!index.isValid()) return QVariant();
	switch(role)
	{
		case Qt::DisplayRole:
		switch(index.column())
		{
			case 0: return rules.at(index.row()).regexp;
			case 1: return rules.at(index.row()).color.name();
			default: return QVariant();
		}
		case Qt::EditRole:
		switch(index.column())
		{
			case 0: return rules.at(index.row()).regexp;
			case 1: return rules.at(index.row()).color.name();
			default: return QVariant();
		}
		case Qt::ToolTipRole:
		switch(index.column())
		{
			case 0: return rules.at(index.row()).regexp;
			case 1: return rules.at(index.row()).color.name();
		}
		case Qt::BackgroundRole:
		switch(index.column())
		{
			case 1: return rules.at(index.row()).color;
			default: return QVariant();
		}
		case Qt::CheckStateRole:
		switch(index.column())
		{
			case 0: return rules.at(index.row()).enabled?Qt::Checked:Qt::Unchecked;
			default: return QVariant();
		}
		default: return QVariant();
	}
	return QVariant();
}

bool HighlightRuleModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
	if(!index.isValid()) return false;
	switch(role)
	{
		case Qt::CheckStateRole:
		switch(index.column())
		{
			case 0:
			{
				rules[index.row()].enabled = value.toBool();
				emit dataChanged(index, index);
				return true;
			}
			default: return QAbstractItemModel::setData(index, value, role);
		}
		case Qt::EditRole:
		switch(index.column())
		{
			case 0:
			{
				rules[index.row()].regexp = value.toString();
				emit dataChanged(index, index);
				return true;
			}
			case 1:
			{
				rules[index.row()].color = value.toString();
				emit dataChanged(index, index);
				return true;
			}
			default: return QAbstractItemModel::setData(index, value, role);
		}
		default: return QAbstractItemModel::setData(index, value, role);
	}
	return QAbstractItemModel::setData(index, value, role);
}

Qt::ItemFlags HighlightRuleModel::flags(const QModelIndex& index) const
{
	if(!index.isValid()) return 0;
	switch(index.column())
	{
		case 0: return  Qt::ItemIsDragEnabled|Qt::ItemIsSelectable|Qt::ItemIsEnabled|Qt::ItemIsUserCheckable|Qt::ItemIsEditable;
		case 1: return  Qt::ItemIsDragEnabled|Qt::ItemIsSelectable|Qt::ItemIsEnabled|Qt::ItemIsEditable;
		default: return Qt::ItemIsDropEnabled|Qt::ItemIsSelectable|Qt::ItemIsEnabled;
	}
}

QModelIndex HighlightRuleModel::index(int row, int column, const QModelIndex&) const
{
	return createIndex(row, column);
}

QModelIndex HighlightRuleModel::parent(const QModelIndex&) const
{
	return QModelIndex();
}

void HighlightRuleModel::appendRow()
{
	beginInsertRows(QModelIndex(),rules.size(),rules.size());
	rules.resize(rules.size()+1);
	endInsertRows();
}

void HighlightRuleModel::removeRow(const QModelIndex& index)
{
	if(!index.isValid()) return;
	beginRemoveRows(index.parent(),index.row(),index.row());
	rules.remove(index.row());
	endRemoveRows();
}

void HighlightRuleModel::up(const QModelIndex& index)
{
	if(!index.isValid()) return;
	int row = index.row();
	beginMoveRows(index.parent(),row,row,index.parent(),row-1);
	qSwap(rules[row],rules[row-1]);
	endMoveRows();
}

void HighlightRuleModel::down(const QModelIndex& index)
{
	if(!index.isValid()) return;
	int row = index.row();
	beginMoveRows(index.parent(),row,row,index.parent(),row+2);
	qSwap(rules[row],rules[row+1]);
	endMoveRows();
}
