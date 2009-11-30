#ifndef TOOLBARMODEL_H
#define TOOLBARMODEL_H

#include <QAbstractListModel>
#include <QVector>

class ItemModel : public QAbstractListModel
{
public:
	ItemModel();
	int rowCount(const QModelIndex & = QModelIndex()) const;
	QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
private:
	QVector<bool> v;
};

class ItemToolbarModel : public QAbstractListModel
{
public:
	ItemToolbarModel();
	int rowCount(const QModelIndex & = QModelIndex()) const;
	QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
	//
	void up(const QModelIndex &index);
	void down(const QModelIndex &index);
	//
	void setVector(const QVector<int>& nv);
	const QVector<int>& getVector() const;
private:
	QVector<int> v;
};

#endif // TOOLBARMODEL_H
