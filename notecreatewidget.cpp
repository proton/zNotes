#include "notecreatewidget.h"
#include "ui_notecreatewidget.h"
#include "notelist.h"
#include "notecreatebutton.h"

#include <QSignalMapper>

NoteCreateWidget::NoteCreateWidget(QWidget *parent, NoteList* note_list) :
	QWidget(parent),
	ui(new Ui::NoteCreateWidget),
	notes(note_list)
{
	ui->setupUi(this);
	signal_mapper = new QSignalMapper(this);
	foreach(const NoteType& note_type, note_list->noteTypes())
	{
		if(!note_type.visible()) continue;
		QPushButton* button = new NoteCreateButton(this, note_type);
		connect(button, SIGNAL(clicked()), signal_mapper, SLOT(map()));
		signal_mapper->setMapping(button, int(note_type.id()));
		ui->noteTypesLayout->addWidget(button);
	}
	connect(signal_mapper, SIGNAL(mapped(int)), this, SLOT(clicked(int)));
}

NoteCreateWidget::~NoteCreateWidget()
{
	delete ui;
}

void NoteCreateWidget::clicked(int id)
{
	notes->create(Note::Type(id), ui->noteNameEdit->text());
	hide();
	emit closed(true);
}

void NoteCreateWidget::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void NoteCreateWidget::on_closeButton_clicked()
{
	hide();
	emit closed(true);
}
