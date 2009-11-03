#include "configdialog.h"
#include "ui_configdialog.h"

#include "settings.h"

#include <QtDebug>
#include <QFileDialog>
#include <QFontDialog>
#include <QColorDialog>

configDialog::configDialog(QWidget *parent) :
	QDialog(parent), m_ui(new Ui::configDialog)
{
	m_ui->setupUi(this);
	m_ui->ed_NotesPath->setText(settings.getNotesPath());
	m_ui->cb_HideStart->setChecked(settings.getHideStart());
	m_ui->cb_ToolbarHide->setChecked(settings.getHideToolbar());
	m_ui->cb_FrameHide->setChecked(settings.getHideFrame());
	m_ui->cb_StayTop->setChecked(settings.getStayTop());
	m_ui->lb_FontExample->setFont(settings.getNoteFont());
	m_ui->tabScripts->setModel(&settings.getScriptModel());
	m_ui->tabScripts->resizeColumnsToContents();
	m_ui->cb_ScriptShowOutput->setChecked(settings.getScriptShowOutput());
	m_ui->cb_ScriptCopyOutput->setChecked(settings.getScriptCopyOutput());
	//
//	m_ui->cb_tbHideEdit->setChecked(settings.getTbHideEdit());
//	m_ui->cb_tbHideMove->setChecked(settings.getTbHideMove());
//	m_ui->cb_tbHideCopy->setChecked(settings.getTbHideCopy());
//	m_ui->cb_tbHideSetup->setChecked(settings.getTbHideSetup());
//	m_ui->cb_tbHideRun->setChecked(settings.getTbHideRun());
//	m_ui->cb_tbHideExit->setChecked(settings.getTbHideExit());
}

configDialog::~configDialog()
{
	delete m_ui;
}

void configDialog::SaveSettings()
{
	settings.setHideStart(m_ui->cb_HideStart->checkState());
	settings.setNotesPath(m_ui->ed_NotesPath->text());
	settings.setHideToolbar(m_ui->cb_ToolbarHide->checkState());
	settings.setHideFrame(m_ui->cb_FrameHide->checkState());
	settings.setStayTop(m_ui->cb_StayTop->checkState());
	settings.setNoteFont(m_ui->lb_FontExample->font());
	settings.setScriptShowOutput(m_ui->cb_ScriptShowOutput->checkState());
	settings.setScriptCopyOutput(m_ui->cb_ScriptCopyOutput->checkState());
	//
//	settings.setTbHideEdit(m_ui->cb_tbHideEdit->checkState());
//	settings.setTbHideMove(m_ui->cb_tbHideMove->checkState());
//	settings.setTbHideCopy(m_ui->cb_tbHideCopy->checkState());
//	settings.setTbHideSetup(m_ui->cb_tbHideSetup->checkState());
//	settings.setTbHideRun(m_ui->cb_tbHideRun->checkState());
//	settings.setTbHideExit(m_ui->cb_tbHideExit->checkState());
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

void configDialog::on_btn_ScriptRemove_clicked()
{
	if(!m_ui->tabScripts->selectionModel()->hasSelection()) return;
	settings.getScriptModel().removeRow(m_ui->tabScripts->selectionModel()->currentIndex().row());
}

void configDialog::on_btn_ScriptAdd_clicked()
{
	settings.getScriptModel().append("","","");
}
