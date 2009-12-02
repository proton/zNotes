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
	//
	void setVector(const QVector<int>& nv);
	//
	void insert(int);
	void remove(int);
	//
	bool isUsed(int row) const;
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
	void setVector(const QVector<int>& nv);
	const QVector<int>& getVector() const;
	//
	void up(const QModelIndex &index);
	void down(const QModelIndex &index);
	//
	inline int getId(const QModelIndex & index) const { return v[index.row()]; }
	//
	void insert(int, int);
	void remove(const QModelIndex &);
private:
	QVector<int> v;
};

#endif // TOOLBARMODEL_H
