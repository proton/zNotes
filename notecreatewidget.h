#ifndef NOTECREATEWIDGET_H
#define NOTECREATEWIDGET_H

#include <QWidget>

class QSignalMapper;

class NoteList;

namespace Ui {
	class NoteCreateWidget;
}

class NoteCreateWidget : public QWidget
{
	Q_OBJECT

public:
	explicit NoteCreateWidget(QWidget* parent, NoteList* note_list);
	~NoteCreateWidget();

protected:
	void changeEvent(QEvent* e);

private slots:
	void clicked(int id);

	void on_closeButton_clicked();

signals:
	void closed(bool);

private:
	Ui::NoteCreateWidget *ui;
	NoteList* notes;
	QSignalMapper* signal_mapper;
};

#endif // NOTECREATEWIDGET_H
