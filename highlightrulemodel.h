#ifndef HIGHLIGHTRULEMODEL_H
#define HIGHLIGHTRULEMODEL_H

#include <QAbstractItemModel>
#include <QVector>

#include "settings.h"

class HighlightRuleModel : public QAbstractItemModel
{
	Q_OBJECT
public:
	explicit HighlightRuleModel(QObject* parent, const QVector<HighlightRule>&);
	int	columnCount(const QModelIndex& parent = QModelIndex()) const;
	int	rowCount(const QModelIndex& parent = QModelIndex()) const;
	QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
	QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;
	bool setData(const QModelIndex& index, const QVariant & value, int role = Qt::EditRole);
	Qt::ItemFlags flags(const QModelIndex& index) const;
	QModelIndex index(int, int, const QModelIndex&) const;
	QModelIndex parent(const QModelIndex&) const;
	void appendRow();
	void removeRow(const QModelIndex&);
	void up(const QModelIndex&);
	void down(const QModelIndex&);
	const QVector<HighlightRule>& getRules() const { return rules; }

signals:

public slots:

private:
	QVector<HighlightRule> rules;

};

#endif // HIGHLIGHTRULEMODEL_H
