#ifndef SCRIPTMODEL_H
#define SCRIPTMODEL_H

#include <QStandardItemModel>

class ScriptModel : public QStandardItemModel
{
public:
	ScriptModel();
	void append(const QString& name = QString(), const QString& file = QString(), const QString& icon = QString());
	const QString getName(int i) { return data(index(i,0)).toString(); }
	const QString getFile(int i) { return data(index(i,1)).toString(); }
	const QString getIcon(int i) { return data(index(i,2)).toString(); }
	QVariant headerData ( int section, Qt::Orientation orientation, int role = Qt::DisplayRole ) const;
};

#endif // SCRIPTMODEL_H
