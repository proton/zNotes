#include "ztabbar.h"

ZTabBar::ZTabBar(QWidget *parent) :
	QTabBar(parent)
{
}

QSize ZTabBar::sizeHint() const
{
	return QTabBar::sizeHint();
//	Q_D(const QTabBar);
//	if (d->layoutDirty)
//		const_cast<QTabBarPrivate*>(d)->layoutTabs();
//	QRect r;
//	for (int i = 0; i < d->tabList.count(); ++i)
//		r = r.united(d->tabList.at(i).maxRect);
//	QSize sz = QApplication::globalStrut();
//	return r.size().expandedTo(sz);
}

QSize ZTabBar::minimumSizeHint() const
{
	return QTabBar::minimumSizeHint();
//	Q_D(const QTabBar);
//	if (!d->useScrollButtons) {
//		QRect r;
//		for (int i = 0; i < d->tabList.count(); ++i)
//			r = r.united(d->tabList.at(i).minRect);
//		return r.size().expandedTo(QApplication::globalStrut());
//	}
//	if (verticalTabs(d->shape))
//		return QSize(sizeHint().width(), d->rightB->sizeHint().height() * 2 + 75);
//	else
//		return QSize(d->rightB->sizeHint().width() * 2 + 75, sizeHint().height());
}

void ZTabBar::paintEvent(QPaintEvent* pe)
{
	QTabBar::paintEvent(pe);
//	Q_D(QTabBar);

//	QStyleOptionTabBarBaseV2 optTabBase;
//	QTabBarPrivate::initStyleBaseOption(&optTabBase, this, size());

//	QStylePainter p(this);
//	int selected = -1;
//	int cut = -1;
//	bool rtl = optTabBase.direction == Qt::RightToLeft;
//	bool vertical = verticalTabs(d->shape);
//	QStyleOptionTab cutTab;
//	selected = d->currentIndex;
//	if (d->dragInProgress)
//		selected = d->pressedIndex;

//	for (int i = 0; i < d->tabList.count(); ++i)
//		 optTabBase.tabBarRect |= tabRect(i);

//	optTabBase.selectedTabRect = tabRect(selected);

//	if (d->drawBase)
//		p.drawPrimitive(QStyle::PE_FrameTabBarBase, optTabBase);

//	for (int i = 0; i < d->tabList.count(); ++i) {
//		QStyleOptionTabV3 tab;
//		initStyleOption(&tab, i);
//		if (d->paintWithOffsets && d->tabList[i].dragOffset != 0) {
//			if (vertical) {
//				tab.rect.moveTop(tab.rect.y() + d->tabList[i].dragOffset);
//			} else {
//				tab.rect.moveLeft(tab.rect.x() + d->tabList[i].dragOffset);
//			}
//		}
//		if (!(tab.state & QStyle::State_Enabled)) {
//			tab.palette.setCurrentColorGroup(QPalette::Disabled);
//		}
//		// If this tab is partially obscured, make a note of it so that we can pass the information
//		// along when we draw the tear.
//		if (((!vertical && (!rtl && tab.rect.left() < 0)) || (rtl && tab.rect.right() > width()))
//			|| (vertical && tab.rect.top() < 0)) {
//			cut = i;
//			cutTab = tab;
//		}
//		// Don't bother drawing a tab if the entire tab is outside of the visible tab bar.
//		if ((!vertical && (tab.rect.right() < 0 || tab.rect.left() > width()))
//			|| (vertical && (tab.rect.bottom() < 0 || tab.rect.top() > height())))
//			continue;

//		optTabBase.tabBarRect |= tab.rect;
//		if (i == selected)
//			continue;

//		p.drawControl(QStyle::CE_TabBarTab, tab);
//	}

//	// Draw the selected tab last to get it "on top"
//	if (selected >= 0) {
//		QStyleOptionTabV3 tab;
//		initStyleOption(&tab, selected);
//		if (d->paintWithOffsets && d->tabList[selected].dragOffset != 0) {
//			if (vertical)
//				tab.rect.moveTop(tab.rect.y() + d->tabList[selected].dragOffset);
//			else
//				tab.rect.moveLeft(tab.rect.x() + d->tabList[selected].dragOffset);
//		}
//		if (!d->dragInProgress)
//			p.drawControl(QStyle::CE_TabBarTab, tab);
//		else {
//			int taboverlap = style()->pixelMetric(QStyle::PM_TabBarTabOverlap, 0, this);
//			d->movingTab->setGeometry(tab.rect.adjusted(-taboverlap, 0, taboverlap, 0));
//		}
//	}

//	// Only draw the tear indicator if necessary. Most of the time we don't need too.
//	if (d->leftB->isVisible() && cut >= 0) {
//		cutTab.rect = rect();
//		cutTab.rect = style()->subElementRect(QStyle::SE_TabBarTearIndicator, &cutTab, this);
//		p.drawPrimitive(QStyle::PE_IndicatorTabTear, cutTab);
//	}
}
