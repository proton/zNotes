#ifndef ZTABWIDGET_H
#define ZTABWIDGET_H

#include <QTabWidget>

class ZTabBar;

class ZTabWidget : public QTabWidget
{
	Q_OBJECT
public:
	explicit ZTabWidget(QWidget *parent = 0);
	
signals:
    void tabBarDoubleClickedOnEmptySpace();
    void tabBarDoubleClickedOnTab(int);
	
public slots:

protected:
    // For detecting working tab
    bool eventFilter(QObject *obj, QEvent *event);
	
private:
    ZTabBar *tabBar;
};

#endif // ZTABWIDGET_H
