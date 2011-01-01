#include "configdialog.h"
#include "ui_configdialog.h"

#include "settings.h"
#include "toolbarmodel.h"

#include <QFileDialog>
#include <QFontDialog>
#include <QColorDialog>

ToolbarItems* t_items;
ItemModel* m_items;
ItemToolbarModel* mt_items;

configDialog::configDialog(QWidget *parent) :
	QDialog(parent), m_ui(new Ui::configDialog)
{
	m_ui->setupUi(this);
#ifndef SINGLE_INSTANCE
	m_ui->cb_SingleInstance->setDisabled(true);
#endif
	t_items = new ToolbarItems();
	m_items = new ItemModel(*t_items);
	mt_items = new ItemToolbarModel(*t_items);
	//
	m_ui->listActions->setModel(m_items);
	m_ui->listActions->setDragEnabled(true);
	m_ui->listActions->setAcceptDrops(true);
	m_ui->listActions->setDropIndicatorShown(true);
	m_ui->listActions->setDragDropMode(QAbstractItemView::DragDrop);
	//
	m_ui->listToolbarActions->setModel(mt_items);
	m_ui->listToolbarActions->setDragEnabled(true);
	m_ui->listToolbarActions->setAcceptDrops(true);
	m_ui->listToolbarActions->setDropIndicatorShown(true);
	m_ui->listToolbarActions->setDragDropMode(QAbstractItemView::DragDrop);
	//
	m_ui->ed_NotesPath->setText(settings.getNotesPath());
	m_ui->cmb_TabPosition->setCurrentIndex(settings.getTabPosition());
	m_ui->cb_ShowHidden->setChecked(settings.getShowHidden());
	m_ui->cb_ShowExtensions->setChecked(settings.getShowExtensions());
	m_ui->cb_HideStart->setChecked(settings.getHideStart());
	m_ui->cb_FrameHide->setChecked(settings.getHideFrame());
	m_ui->cb_StayTop->setChecked(settings.getStayTop());
	//
#ifdef SINGLE_INSTANCE
	m_ui->cb_SingleInstance->setChecked(settings.getSingleInstance());
	m_ui->cb_CopyStartRaise->setChecked(settings.getCopyStartRaise());
#endif
	//
	m_ui->tabScripts->setModel(&settings.getScriptModel());
	m_ui->tabScripts->resizeColumnsToContents();
	m_ui->cb_ScriptShowOutput->setChecked(settings.getScriptShowOutput());
	m_ui->cb_ScriptCopyOutput->setChecked(settings.getScriptCopyOutput());
	//
	m_ui->lb_FontExample->setFont(settings.getNoteFont());
	m_ui->cb_NoteLinksHighlight->setChecked(settings.getNoteLinksHighlight());
	m_ui->cb_NoteLinksOpen->setChecked(settings.getNoteLinksOpen());
	m_ui->cb_NotePastePlaintext->setChecked(settings.getNotePastePlaintext());
	//
	const QMap<int, QMap<int, QString> >& translations = settings.getTranslations();
	QMapIterator<int, QMap<int, QString> > translation(translations);
	while(translation.hasNext())
	{
		translation.next();
		QLocale::Language language = QLocale::Language(translation.key());
		QMapIterator<int, QString> country_it(translation.value());
		while(country_it.hasNext())
		{
			QLocale::Country country = QLocale::Country(country_it.next().key());
			QLocale locale(language, country);
			QString text = QString("%1 (%2)").
				arg(QLocale::languageToString(language)).
				arg(QLocale::countryToString(country));
			m_ui->cmb_Language->addItem(text, locale);
		}
	}
	int current_locale_index = m_ui->cmb_Language->findData(settings.getLocaleCurrent());
	m_ui->cmb_Language->setCurrentIndex(current_locale_index);
	//
	m_ui->cb_LanguageCustom->setChecked(settings.getLanguageCustom());
	m_ui->cmb_Language->setEnabled(settings.getLanguageCustom());
	//
	t_items->setVector(settings.getToolbarItems());
	//
	connect(m_ui->listActions->selectionModel(), SIGNAL(currentRowChanged(QModelIndex,QModelIndex)), this, SLOT(currentListActionChanged(QModelIndex,QModelIndex))); //TODO: selection changed
	connect(m_ui->listToolbarActions->selectionModel(), SIGNAL(currentRowChanged(QModelIndex,QModelIndex)), this, SLOT(currentToolbarActionChanged(QModelIndex,QModelIndex)));
}

configDialog::~configDialog()
{
	delete m_ui;
}

void configDialog::SaveSettings()
{
	settings.setHideStart(m_ui->cb_HideStart->checkState());
	settings.setNotesPath(m_ui->ed_NotesPath->text());
	settings.setTabPosition(TabPosition(m_ui->cmb_TabPosition->currentIndex()));
	settings.setShowHidden(m_ui->cb_ShowHidden->checkState());
	settings.setShowExtensions(m_ui->cb_ShowExtensions->checkState());
	settings.setHideFrame(m_ui->cb_FrameHide->checkState());
	settings.setStayTop(m_ui->cb_StayTop->checkState());
#ifdef SINGLE_INSTANCE
	settings.setSingleInstance(m_ui->cb_SingleInstance->checkState());
	settings.setCopyStartRaise(m_ui->cb_CopyStartRaise->checkState());
#endif
	settings.setNoteFont(m_ui->lb_FontExample->font());
	settings.setNoteLinksHighlight(m_ui->cb_NoteLinksHighlight->checkState());
	settings.setNoteLinksOpen(m_ui->cb_NoteLinksOpen->checkState());
	settings.setNotePastePlaintext(m_ui->cb_NotePastePlaintext->checkState());
	settings.setScriptShowOutput(m_ui->cb_ScriptShowOutput->checkState());
	settings.setScriptCopyOutput(m_ui->cb_ScriptCopyOutput->checkState());
	settings.setToolbarItems(t_items->getToolbarElems());
	settings.setLocaleCustom(m_ui->cb_LanguageCustom->isChecked());
	settings.setLocaleCurrent(m_ui->cmb_Language->itemData(m_ui->cmb_Language->currentIndex(), Qt::UserRole).toLocale());
}

void configDialog::on_buttonBox_clicked(QAbstractButton* button)
{
	const QAbstractButton* BTN_SAVE = m_ui->buttonBox->buttons().at(0);
	const QAbstractButton* BTN_CLOSE = m_ui->buttonBox->buttons().at(1);
	const QAbstractButton* BTN_APPLY = m_ui->buttonBox->buttons().at(2);
	//
	if(button==BTN_SAVE)
	{
		SaveSettings();
		close();
	}
	else if(button==BTN_CLOSE)
	{
		close();
	}
	else if(button==BTN_APPLY)
	{
		SaveSettings();
	}
}

void configDialog::on_btn_NotesPath_clicked()
{
	QString path = settings.getNotesPath();
	path = QFileDialog::getExistingDirectory(0,
			tr("Select notes directory"),settings.getNotesPath(),
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
	int row = m_ui->tabScripts->selectionModel()->currentIndex().row();
	settings.getScriptModel().removeRow(row);
}

void configDialog::on_btn_ScriptAdd_clicked()
{
	settings.getScriptModel().append();
}

void configDialog::on_butActionAdd_clicked()
{
	int id = m_ui->listActions->currentIndex().row();
	int row = (m_ui->listToolbarActions->selectionModel()->hasSelection())?
		m_ui->listToolbarActions->currentIndex().row():-1;
	t_items->insert(id, row);
}

void configDialog::on_butActionRemove_clicked()
{
	m_ui->butActionRemove->setDisabled(true);
	QModelIndex index = m_ui->listToolbarActions->currentIndex();
	t_items->remove(index.row());
}

void configDialog::on_butActionTop_clicked()
{
	QModelIndex index(m_ui->listToolbarActions->currentIndex());
	QModelIndex new_index = mt_items->up(index);
	m_ui->listToolbarActions->setCurrentIndex(new_index);
}

void configDialog::on_butActionBottom_clicked()
{
	QModelIndex index(m_ui->listToolbarActions->currentIndex());
	QModelIndex new_index = mt_items->down(index);
	m_ui->listToolbarActions->setCurrentIndex(new_index);
}

//On changing selection in toolar actions list
void configDialog::currentToolbarActionChanged(QModelIndex index, QModelIndex)
{
	m_ui->butActionRemove->setEnabled(index.isValid());
	int row = index.row();
	m_ui->butActionTop->setEnabled(row>0);
	m_ui->butActionBottom->setEnabled(row<(mt_items->rowCount()-1));
}

//On changing selection in current toolar actions list
void configDialog::currentListActionChanged(QModelIndex index, QModelIndex)
{
//	QModelIndexList list = m_ui->listActions->selectionModel()->selectedRows(0);
//	qDebug() << "> " << list.size();
//	for(int i=0; i<list.size(); ++i)
//	{if(m_items.isUsed(list[i].row()))
//	{
//		m_ui->butActionAdd->setEnabled(false);
//		return;
//	} qDebug() << i; }
//	m_ui->butActionAdd->setEnabled(true);
	m_ui->butActionAdd->setEnabled(index.isValid() && !t_items->isUsed(index.row()));
}

//Retranslating ui on language change
void configDialog::changeEvent(QEvent *e)
{
	QDialog::changeEvent(e);
	switch (e->type()) {
	case QEvent::LanguageChange:
		m_ui->retranslateUi(this);
		//retranslateUi() function changes current index of cmb_TabPosition
		m_ui->cmb_TabPosition->setCurrentIndex(settings.getTabPosition());
		break;
	default:
		break;
	}
}

