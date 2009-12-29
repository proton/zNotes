#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "settings.h"
#include "configdialog.h"
#include "aboutDialog.h"

#include <QtDebug>
#include <QInputDialog>
#include <QMessageBox>
#include <QFileDialog>
#include <QClipboard>
#include <QProcess>

/*
	Tray icon on windows is very small
*/
#ifdef unix
#define TRAY_ICON_FILE_NAME ":/res/znotes32.png"
#else
#define TRAY_ICON_FILE_NAME ":/res/znotes.png"
#endif

Settings settings;

void MainWindow::RemoveCurrentNote()
{
	if(Notes.count()<1) return;
	Note* n = currentNote();
	QMessageBox msgBox(QMessageBox::Question, tr("Delete Note"),
		tr("Do you realy want to delete note %1 ?").arg(n->name),
		QMessageBox::Yes | QMessageBox::No);
	int ret = msgBox.exec();
	if(ret == QMessageBox::Yes)
	{
		n->file.close();
		n->file.remove(dir.absoluteFilePath(n->name));
		Notes.remove(ui->tabs->currentIndex());
		ui->tabs->removeTab(ui->tabs->currentIndex());
		delete n;
		if(Notes.count()<=1)
		{
			actRemove->setDisabled(true);
			cmenu.actions()[4]->setDisabled(true);
		}
	}
}

void MainWindow::RenameCurrentNote()
{
	if(Notes.count()<1) return;
	SaveCurrentNote();
	Note* n = currentNote();
	bool ok;
	QString text = QInputDialog::getText(this, tr("Rename note"),
							tr("New name:"), QLineEdit::Normal, n->name, &ok);
	if (ok && !text.isEmpty())
	{
		n->file.close();
		n->file.rename(dir.absoluteFilePath(text));
		n->name = text;
		ui->tabs->setTabText(ui->tabs->currentIndex(), text);
	}
}

void MainWindow::SaveCurrentNote()
{
	SaveNote(CurrentIndex);
}

void MainWindow::SaveNote(int i)
{
	if(i!=-1 && Notes[i]->hasChange)
	{
		if(!Notes[i]->file.open(QFile::WriteOnly | QFile::Text)) return;
		QTextStream out(&Notes[i]->file);
		out << Notes[i]->toPlainText();
		Notes[i]->file.close();
		Notes[i]->hasChange = false;
	}
}

void MainWindow::NewNote()
{
	int n = Notes.size(), index = Notes.size();
	QString fn = QString::number(n);
	QFile f(dir.absoluteFilePath(fn));
	while(f.exists())
	{
		fn = QString::number(++n);
		f.setFileName(dir.absoluteFilePath(fn));
	}
	Notes.append(new Note(fn, dir, settings.getNoteFont()));
	ui->tabs->addTab(Notes[index], fn);
	ui->tabs->setCurrentIndex(index);
	QObject::connect(Notes[index], SIGNAL(textChanged()), this, SLOT(currentNoteChanged()));
	if(Notes.count()>1)
	{
		actRemove->setEnabled(true);
		cmenu.actions()[4]->setEnabled(true);
	}
}

void MainWindow::PreviousNote()
{
	ui->tabs->setCurrentIndex(ui->tabs->currentIndex()-1);
}

void MainWindow::NextNote()
{
	ui->tabs->setCurrentIndex(ui->tabs->currentIndex()+1);
}

void MainWindow::ToNote(int n)
{
	if(n>=Notes.size()) return;
	ui->tabs->setCurrentIndex(n);
}

void MainWindow::CopyNote()
{
	QApplication::clipboard()->setText(currentNote()->toPlainText());
}

void MainWindow::LoadNotes()
{
	ui->tabs->clear();
	dir.setPath(settings.getNotesPath());
	if(!dir.exists()) if(!dir.mkpath(dir.path())) dir.setPath("");
	if(!dir.isReadable()) dir.setPath("");
	while(dir.path().isEmpty())
	{
		dir.setPath(QFileDialog::getExistingDirectory(0,
			tr("Select notes directory"), QDir::homePath(),
			QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks));
		if(!dir.path().isEmpty()) settings.setNotesPath(dir.path());
	}
	dir.setFilter(QDir::Files | QDir::Readable);
	QFileInfoList flist = dir.entryInfoList();
	Notes.resize(flist.size());
	const QString& old_note = settings.getLastNote();
	int old_index=-1;
	for(int i=0; i<flist.size(); ++i)
	{
		Notes[i] = new Note(flist.at(i).fileName(), dir, settings.getNoteFont());
		ui->tabs->addTab(Notes[i], Notes[i]->name);
		QObject::connect(Notes[i], SIGNAL(textChanged()), this, SLOT(currentNoteChanged()));
		if(old_index==-1 && Notes[i]->name==old_note) old_index = i;
	}
	if(old_index!=-1) ui->tabs->setCurrentIndex(old_index);
}

void MainWindow::currentNoteChanged()
{
	currentNote()->hasChange = true;
}

void MainWindow::SaveAll()
{
	for(int i=0; i<Notes.size(); ++i)
	{
		SaveNote(i);
	}
}

void MainWindow::trayActivated(QSystemTrayIcon::ActivationReason reason)
{
	switch(reason)
	{
		case QSystemTrayIcon::Trigger:
		{
			if(isHidden()) show();
			else hide();
			break;
		}
		default: break;
	}
}

void MainWindow::hideEvent(QHideEvent */*event*/)
{
	cmenu.actions()[0]->setEnabled(true);
	cmenu.actions()[1]->setDisabled(true);
	SaveAll();
}

void MainWindow::showEvent(QShowEvent */*event*/)
{
	cmenu.actions()[0]->setEnabled(false);
	cmenu.actions()[1]->setDisabled(false);
}

void MainWindow::closeEvent(QCloseEvent *event)
{
	if(tray.isVisible())
	{
		hide();
		event->ignore();
	}
}

void MainWindow::showAboutDialog()
{
	aboutDialog dlg;
	dlg.exec();
}

void MainWindow::showPrefDialog()
{
	configDialog dlg;
	dlg.exec();
}

void MainWindow::notesPathChanged()
{
	QMessageBox msgBox(QMessageBox::Question, tr("Move Notes"),
		tr("Notes path changed!\nDo you want to move your notes to new place ?"),
		QMessageBox::Yes | QMessageBox::No);
	int ret = msgBox.exec();
	if(ret == QMessageBox::Yes)
	{
		dir.setPath(settings.getNotesPath());
		for(int i=0; i<Notes.count(); ++i)
			Notes[i]->file.rename(dir.absoluteFilePath(Notes[i]->name));
	}
	else QMessageBox::information(this, tr("Notes path change"),
		tr("You need restart application to get effect."));
}

void MainWindow::windowStateChanged()
{
	Qt::WindowFlags flags = Qt::Window;
	if(settings.getHideFrame()) flags |= Qt::FramelessWindowHint;
	if(settings.getStayTop()) flags |= Qt::WindowStaysOnTopHint;
	bool window_is_visible = isVisible();
	setWindowFlags(flags);
	if(window_is_visible) show();
}

//void MainWindow::toolbarVisChanged()
//{
//	if(settings.getHideToolbar()) ui->mainToolBar->hide();
//	else ui->mainToolBar->show();
//}

void MainWindow::noteFontChanged()
{
	for(int i=0; i<Notes.count(); ++i)
	{
		Notes[i]->setFont(settings.getNoteFont());
	}
}

void MainWindow::commandMenu()
{
	if(cmd_menu.actions().size()>0)
	{
		QPoint p = ui->mainToolBar->actionGeometry(actRun).center()+pos();
		cmd_menu.exec(p);
	}
	else
	{
		QMessageBox(QMessageBox::Information, tr("Commandlist is clear"), tr("List of commands is clear!\nYou can add new commands in preferences.")).exec();
	}
}

void MainWindow::cmdExec(const QString & command)
{
	QProcess p;
	QStringList args;
	args << dir.absoluteFilePath(currentNote()->file.fileName());
	p.start(command, args);
	if(p.waitForStarted())
	{
		if(p.waitForFinished(10000))
		{
			QByteArray result = p.readAll();
			if(settings.getScriptShowOutput()) tray.showMessage(command, QString::fromUtf8(result));
			if(settings.getScriptCopyOutput()) QApplication::clipboard()->setText(QString::fromUtf8(result));
		}
	}
}

void MainWindow::cmd_changed()
{
	cmd_menu.clear();
	ScriptModel& sm = settings.getScriptModel();
	for(int i=0; i<sm.rowCount(); ++i)
	{
		cmd_menu.addAction(QIcon(sm.getIcon(i)), sm.getName(i), &cmd_mapper, SLOT(map()));
		cmd_mapper.setMapping(cmd_menu.actions().last(), sm.getFile(i));
		connect(&cmd_mapper, SIGNAL(mapped(const QString &)), this, SLOT(cmdExec(const QString &)));
	}
}

void MainWindow::actions_changed()
{
	ui->mainToolBar->clear();
	const QVector<int>& items = settings.getToolbarItems();
	for(int i=0; i<items.size(); ++i)
	{
		if(items[i]==itemSeparator) ui->mainToolBar->addSeparator();
		else ui->mainToolBar->addAction(getAction(items[i]));
	}
}

MainWindow::MainWindow(QWidget *parent)
	: QMainWindow(parent), ui(new Ui::MainWindow), CurrentIndex(-1)
{
	settings.load();
	ui->setupUi(this);
	ui->wSearch->hide();
	//restoring window state
	restoreGeometry(settings.getDialogGeometry());
	restoreState(settings.getDialogState());
	windowStateChanged();
	//
	actAdd = new QAction(ToolbarAction(itemAdd).icon(), ToolbarAction(itemAdd).text(), parent);
	actRemove = new QAction(ToolbarAction(itemRemove).icon(), ToolbarAction(itemRemove).text(), parent);
	actRename = new QAction(ToolbarAction(itemRename).icon(), ToolbarAction(itemRename).text(), parent);
	actPrev = new QAction(ToolbarAction(itemPrev).icon(), ToolbarAction(itemPrev).text(), parent);
	actNext = new QAction(ToolbarAction(itemNext).icon(), ToolbarAction(itemNext).text(), parent);
	actCopy = new QAction(ToolbarAction(itemCopy).icon(), ToolbarAction(itemCopy).text(), parent);
	actSetup = new QAction(ToolbarAction(itemSetup).icon(), ToolbarAction(itemSetup).text(), parent);
	actInfo = new QAction(ToolbarAction(itemInfo).icon(), ToolbarAction(itemInfo).text(), parent);
	actRun = new QAction(ToolbarAction(itemRun).icon(), ToolbarAction(itemRun).text(), parent);
	actSearch = new QAction(ToolbarAction(itemSearch).icon(), ToolbarAction(itemSearch).text(), parent);
	actExit = new QAction(ToolbarAction(itemExit).icon(), ToolbarAction(itemExit).text(), parent);
	//
	connect(actAdd, SIGNAL(triggered()), this, SLOT(NewNote()));
	connect(actRemove, SIGNAL(triggered()), this, SLOT(RemoveCurrentNote()));
	connect(actRename, SIGNAL(triggered()), this, SLOT(RenameCurrentNote()));
	connect(actPrev, SIGNAL(triggered()), this, SLOT(PreviousNote()));
	connect(actNext, SIGNAL(triggered()), this, SLOT(NextNote()));
	connect(actCopy, SIGNAL(triggered()), this, SLOT(CopyNote()));
	connect(actSetup, SIGNAL(triggered()), this, SLOT(showPrefDialog()));
	connect(actInfo, SIGNAL(triggered()), this, SLOT(showAboutDialog()));
	connect(actRun, SIGNAL(triggered()), this, SLOT(commandMenu()));
	connect(actSearch, SIGNAL(triggered()), this, SLOT(showSearchBar()));
	connect(actExit, SIGNAL(triggered()), qApp, SLOT(quit()));
	//
	actions_changed(); //Adding toolbar's actions
	cmd_changed(); //Adding scripts
	//
	cmenu.addAction(tr("Show"), this, SLOT(show()));
	cmenu.addAction(tr("Hide"), this, SLOT(hide()));
	cmenu.addSeparator();
	cmenu.addAction(ToolbarAction(itemAdd).icon(), ToolbarAction(itemAdd).text(), this, SLOT(NewNote()));
	cmenu.addAction(ToolbarAction(itemRemove).icon(), ToolbarAction(itemRemove).text(), this, SLOT(RemoveCurrentNote()));
	cmenu.addAction(ToolbarAction(itemRename).icon(), ToolbarAction(itemRename).text(), this, SLOT(RenameCurrentNote()));
	cmenu.addSeparator();
	cmenu.addAction(ToolbarAction(itemSetup).icon(), ToolbarAction(itemSetup).text(), this, SLOT(showPrefDialog()));
	cmenu.addAction(ToolbarAction(itemInfo).icon(), ToolbarAction(itemInfo).text(), this, SLOT(showAboutDialog()));
	cmenu.addSeparator();
	cmenu.addAction(ToolbarAction(itemExit).icon(), ToolbarAction(itemExit).text(), qApp, SLOT(quit()));
	tray.setIcon(QIcon(TRAY_ICON_FILE_NAME));
	tray.setContextMenu(&cmenu);
	connect(&tray, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
			this, SLOT(trayActivated(QSystemTrayIcon::ActivationReason)));
	tray.show();
	//
	scAdd = new QShortcut(QKeySequence::New, this);
	scRemove = new QShortcut(QKeySequence::Delete, this);
	scPrev = new QShortcut(QKeySequence::Back, this);
	scNext = new QShortcut(QKeySequence::Forward, this);
	scSearch = new QShortcut(QKeySequence::Find, this);
	scExit = new QShortcut(QKeySequence::Close, this);
	//
	connect(scAdd, SIGNAL(activated()), this, SLOT(NewNote()));
	connect(scRemove, SIGNAL(activated()), this, SLOT(RemoveCurrentNote()));
	connect(scPrev, SIGNAL(activated()), this, SLOT(PreviousNote()));
	connect(scNext, SIGNAL(activated()), this, SLOT(NextNote()));
	connect(scSearch, SIGNAL(activated()), this, SLOT(showSearchBar()));
	connect(scExit, SIGNAL(activated()), qApp, SLOT(quit()));
	//
	for(int i=1; i<=9; ++i) //from Alt+1 to Alt+9
	{
		QShortcut* shortcut = new QShortcut(QKeySequence(Qt::ALT + Qt::Key_0+i), this);
		connect(shortcut, SIGNAL(activated()), &alt_mapper, SLOT(map()));
		alt_mapper.setMapping(shortcut, i-1);
	}
	connect(&alt_mapper, SIGNAL(mapped(int)), this, SLOT(ToNote(int)));
	//
	LoadNotes();
	if(Notes.count()==0) NewNote();
	if(Notes.count()<2)
	{
		ui->mainToolBar->actions()[1]->setDisabled(true);
		cmenu.actions()[4]->setDisabled(true);
	}
	//
	connect(&SaveTimer, SIGNAL(timeout()), this, SLOT(SaveAll()));
	SaveTimer.start(10000);
	//
	//connect(ui->mainToolBar->toggleViewAction(), SIGNAL(triggered()), ui->mainToolBar, SLOT(show()));
	connect(&settings, SIGNAL(NotesPathChanged()), this, SLOT(notesPathChanged()));
	connect(&settings, SIGNAL(WindowStateChanged()), this, SLOT(windowStateChanged()));
	//connect(&settings, SIGNAL(ToolbarVisChanged()), this, SLOT(toolbarVisChanged()));
	connect(&settings, SIGNAL(NoteFontChanged()), this, SLOT(noteFontChanged()));
	connect(&settings, SIGNAL(ToolbarItemsChanged()), this, SLOT(actions_changed()));
	connect(&settings.getScriptModel(), SIGNAL(dataChanged(const QModelIndex &, const QModelIndex &)),
		this, SLOT(cmd_changed()));
	if(!settings.getHideStart()) show();
}

MainWindow::~MainWindow()
{
	delete ui;
	//saving notes
	SaveAll();
	//saving title of last note
	settings.setLastNote(currentNote()->name);
	//saving dialog's params
	settings.setDialogGeometry(saveGeometry());
	settings.setDialogState(saveState());
	//saving scrits
	settings.setScripts();
}

void MainWindow::on_tabs_currentChanged(int index)
{
	SaveNote(CurrentIndex);
	CurrentIndex = index;
	actPrev->setDisabled(index==0);
	actNext->setDisabled(index==Notes.count()-1);
}

void MainWindow::showSearchBar()
{
	ui->wSearch->show();
	ui->edSearch->setFocus();
}

/*
  Searching text in notes
*/
void MainWindow::Search(bool next)
{
	QString text = ui->edSearch->text();
	if(text.isEmpty()) return;
	//
	int start_index = CurrentIndex;
	//
	if(!next) Notes[start_index]->unsetCursor();
	//
	if(Notes[start_index]->find(text)) return;
	//
	for(int i=start_index+1; i<Notes.size(); ++i)
	{
		Notes[i]->setTextCursor(QTextCursor());
		if(Notes[i]->find(text))
		{
			ui->tabs->setCurrentIndex(i);
			return;
		}
	}
	for(int i=0; i<start_index; ++i)
	{
		Notes[i]->setTextCursor(QTextCursor());
		if(Notes[i]->find(text))
		{
			ui->tabs->setCurrentIndex(i);
			return;
		}
	}
}

void MainWindow::on_edSearch_textChanged(QString text)
{
	Q_UNUSED(text);
	Search(false);
}

void MainWindow::on_edSearch_returnPressed()
{
	Search(true);
}

void MainWindow::changeEvent(QEvent *e)
{
	QMainWindow::changeEvent(e);
	switch (e->type())
	{
	case QEvent::LanguageChange:
		ui->retranslateUi(this);
		//
		actAdd->setText(ToolbarAction(itemAdd).text());
		actRemove->setText(ToolbarAction(itemRemove).text());
		actRename->setText(ToolbarAction(itemRename).text());
		actPrev->setText(ToolbarAction(itemPrev).text());
		actNext->setText(ToolbarAction(itemNext).text());
		actCopy->setText(ToolbarAction(itemCopy).text());
		actSetup->setText(ToolbarAction(itemSetup).text());
		actInfo->setText(ToolbarAction(itemInfo).text());
		actRun->setText(ToolbarAction(itemRun).text());
		actSearch->setText(ToolbarAction(itemSearch).text());
		actExit->setText(ToolbarAction(itemExit).text());
		//
		cmenu.actions()[0]->setText(tr("Show"));
		cmenu.actions()[1]->setText(tr("Hide"));
		cmenu.actions()[3]->setText(ToolbarAction(itemAdd).text());
		cmenu.actions()[4]->setText(ToolbarAction(itemRemove).text());
		cmenu.actions()[5]->setText(ToolbarAction(itemRename).text());
		cmenu.actions()[7]->setText(ToolbarAction(itemSetup).text());
		cmenu.actions()[8]->setText(ToolbarAction(itemInfo).text());
		cmenu.actions()[10]->setText(ToolbarAction(itemExit).text());
		//
		break;
	default: break;
	}
}

