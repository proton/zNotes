#include "notecreatebutton.h"
#include "notelist.h"

#include <QPainter>
#include <QPaintEvent>

NoteCreateButton::NoteCreateButton(QWidget *parent, const NoteType& ntype) :
	QPushButton(parent), note_type(ntype)
{
	setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
	setIcon(ntype.icon());
	setIconSize(QSize(64,64));
	QString title = QString("%1\n\n%2").arg(note_type.title()).arg(note_type.description());
	setText(title);
}

//void NoteCreateButton::paintEvent(QPaintEvent* e)
//{
//	QPushButton::paintEvent(e);
////	QPainter painter(this);
////	painter.save();
////	QPoint c = e->rect().center();
////	QPoint p;

////	//QFont sub(QApplication::font());
////	QFont sub;
////	sub.setPointSize(sub.pointSize() + 7);
////	painter.setFont(sub);
////	p = QPoint(c.x(), sub.pointSize());
////	painter.drawText(p, note_type.title());

////	QPixmap pixmap(note_type.icon().pixmap(64));
////	p = QPoint(0, c.y()-pixmap.rect().center().y());
////	painter.drawPixmap(p, pixmap);

////	painter.restore();
//}
