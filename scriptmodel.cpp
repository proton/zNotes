#include "scriptmodel.h"

ScriptModel::ScriptModel()
{
	setColumnCount(3);
	setHeaderData(0, Qt::Horizontal, tr("Name"));
	setHeaderData(1, Qt::Horizontal, tr("File"));
	setHeaderData(2, Qt::Horizontal, tr("Icon"));
}

void ScriptModel::append(const QString& name, const QString& file, const QString& icon)
{
	int r = rowCount();
	insertRow(r);
	setData(index(r, 0), name);
	setData(index(r, 1), file);
	setData(index(r, 2), icon);
}
