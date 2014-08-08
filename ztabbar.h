#ifndef ZTABBAR_H
#define ZTABBAR_H

#include <QTabBar>

class ZTabBar : public QTabBar
{
	Q_OBJECT
public:
	explicit ZTabBar(QWidget *parent = 0);

	QSize sizeHint() const;
	QSize minimumSizeHint() const;
	
signals:
	
public slots:

protected:
	void paintEvent(QPaintEvent *);	
};

#endif // ZTABBAR_H
