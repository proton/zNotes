#include "configdialog.h"
#include "ui_configdialog.h"

#include "settings.h"

#include <QtDebug>
#include <QFileDialog>

configDialog::configDialog(QWidget *parent) :
	QDialog(parent),
	m_ui(new Ui::configDialog)
{
	m_ui->setupUi(this);
	m_ui->ed_NotesPath->setText(Settings::getNotesPath());
	m_ui->cb_HideStart->setChecked(Settings::getHideStart());
}

configDialog::~configDialog()
{
	delete m_ui;
}

void configDialog::changeEvent(QEvent *e)
{
	QDialog::changeEvent(e);
	switch (e->type())
	{
		case QEvent::LanguageChange:
			m_ui->retranslateUi(this);
			break;
		default: break;
	}
}

void configDialog::SaveSettings()
{
	Settings::setHideStart(m_ui->cb_HideStart->checkState());

	//FIXME:dirty hack
	if(m_ui->ed_NotesPath->text()!=Settings::getNotesPath())
	{
		Settings::setNotesPath(m_ui->ed_NotesPath->text());
		emit pathChanged();
	}
}

void configDialog::on_buttonBox_clicked(QAbstractButton* button)
{
	if(button==m_ui->buttonBox->buttons().at(0))
	{
		SaveSettings();
		close();
	}
	else if(button==m_ui->buttonBox->buttons().at(2))
	{
		SaveSettings();
	}
	else close();
}

void configDialog::on_btn_NotesPath_clicked()
{
	QString path = Settings::getNotesPath();
	path = QFileDialog::getExistingDirectory(0,
			QObject::tr("Select notes directory"),Settings::getNotesPath(),
			QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
	if(!path.isEmpty()) m_ui->ed_NotesPath->setText(path);
}
