#include "configdialog.h"
#include "ui_configdialog.h"

#include "settings.h"

#include <QtDebug>
#include <QFileDialog>
#include <QFontDialog>
#include <QColorDialog>

configDialog::configDialog(QWidget *parent) :
	QDialog(parent),
	m_ui(new Ui::configDialog)
{
	m_ui->setupUi(this);
	m_ui->ed_NotesPath->setText(settings.getNotesPath());
	m_ui->cb_HideStart->setChecked(settings.getHideStart());
	m_ui->cb_ToolbarHide->setChecked(settings.getHideToolbar());
	m_ui->cb_FrameHide->setChecked(settings.getHideFrame());
	m_ui->cb_StayTop->setChecked(settings.getStayTop());
	m_ui->lb_FontExample->setFont(settings.getNoteFont());
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
	settings.setHideStart(m_ui->cb_HideStart->checkState());
	settings.setNotesPath(m_ui->ed_NotesPath->text());
	settings.setHideToolbar(m_ui->cb_ToolbarHide->checkState());
	settings.setHideFrame(m_ui->cb_FrameHide->checkState());
	settings.setStayTop(m_ui->cb_StayTop->checkState());
	settings.setNoteFont(m_ui->lb_FontExample->font());
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
	QString path = settings.getNotesPath();
	path = QFileDialog::getExistingDirectory(0,
			QObject::tr("Select notes directory"),settings.getNotesPath(),
			QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
	if(!path.isEmpty()) m_ui->ed_NotesPath->setText(path);
}

void configDialog::on_btn_FontChange_clicked()
{
	QFontDialog dlg(m_ui->lb_FontExample->font());
	dlg.exec();
	m_ui->lb_FontExample->setFont(dlg.currentFont());
}
