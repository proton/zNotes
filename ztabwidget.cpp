#include <QMouseEvent>
#include "ztabwidget.h"
#include "ztabbar.h"

ZTabWidget::ZTabWidget(QWidget *parent) :
	QTabWidget(parent)
{
    tabBar = new ZTabBar(this);
    // Control double-click on tab bar through an event filter.
    tabBar->installEventFilter(this);
    setTabBar(tabBar);
}

// Method is based on code from class MyTabWidget of project https://gitorious.org/qmpq.
bool ZTabWidget::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() == QEvent::MouseButtonDblClick) {
        QMouseEvent * mouseEvent = static_cast<QMouseEvent *>(event);
        int tabIndex;
        tabIndex = tabBar->tabAt(mouseEvent->pos());
        if (tabIndex == -1)
            // Chosen no a tab.
            emit tabBarDoubleClickedOnEmptySpace();
        else
            // Chosen a tab
            emit tabBarDoubleClickedOnTab(tabIndex);
        return true;
    } else {
        // standard event processing
        return QTabWidget::eventFilter(obj, event);
    }
}
