#include "scriptmodel.h"

ScriptModel::ScriptModel()
	: QStandardItemModel()
{
	setColumnCount(3);
}

void ScriptModel::append(const QString& name, const QString& file, const QString& icon)
{
	int r = rowCount();
	insertRow(r);
	setData(index(r, 0), name);
	setData(index(r, 1), file);
	setData(index(r, 2), icon);
}

QVariant ScriptModel::headerData ( int section, Qt::Orientation orientation, int role) const
{
	switch(role)
	{
		case Qt::DisplayRole:
		{
			switch(orientation)
			{
				case Qt::Horizontal:
				{
					switch(section)
					{
					case 0: return QObject::tr("Name");
					case 1: return QObject::tr("File");
					case 2: return QObject::tr("Icon");
					default: return QVariant();
					}
				}
				case Qt::Vertical: return QString::number(section);
			}
		}
		default: return QVariant();
	}
}
