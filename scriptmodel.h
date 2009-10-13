#ifndef SCRIPTMODEL_H
#define SCRIPTMODEL_H

#include <QStandardItemModel>

class ScriptModel : public QStandardItemModel
{
public:
	ScriptModel();
	void append(const QString& name, const QString& file, const QString& icon);
	const QString getName(int i) { return data(index(i,0)).toString(); }
	const QString getFile(int i) { return data(index(i,1)).toString(); }
	const QString getIcon(int i) { return data(index(i,2)).toString(); }
};

#endif // SCRIPTMODEL_H
