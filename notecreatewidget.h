#ifndef NOTECREATEWIDGET_H
#define NOTECREATEWIDGET_H

#include <QFrame>

class QSignalMapper;

class NoteList;

namespace Ui {
	class NoteCreateWidget;
}

class NoteCreateWidget : public QFrame
{
	Q_OBJECT

public:
	explicit NoteCreateWidget(QWidget* parent, NoteList* note_list);
	~NoteCreateWidget();

protected:
	void changeEvent(QEvent* e);

private slots:
	void clicked(int id);

public slots:
	void closeEvent(QCloseEvent* event);

signals:
	void closed(bool);

private:
	Ui::NoteCreateWidget *ui;
	NoteList* notes;
	QSignalMapper* signal_mapper;
};

#endif // NOTECREATEWIDGET_H
