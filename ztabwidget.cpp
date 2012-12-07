#include "ztabwidget.h"
#include "ztabbar.h"

ZTabWidget::ZTabWidget(QWidget *parent) :
	QTabWidget(parent)
{
	setTabBar(new ZTabBar(this));
}

