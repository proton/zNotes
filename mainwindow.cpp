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

Settings settings;

void MainWindow::RemoveCurrentNote()
{
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
		if(Notes.count()<2)
		{
			ui->mainToolBar->actions()[1]->setDisabled(true);
			cmenu.actions()[4]->setDisabled(true);
		}
	}
}

void MainWindow::RenameCurrentNote()
{
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
		ui->mainToolBar->actions()[1]->setEnabled(true);
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

void MainWindow::CopyNote()
{
	QApplication::clipboard()->setText(currentNote()->toPlainText());
}

void MainWindow::LoadNotes()
{
	ui->tabs->clear();
	dir.setPath(settings.getNotesPath());
#ifdef unix
	if(dir.path().isEmpty())
	{
		dir.setPath(dir.homePath()+"/.local/share/notes");
		if(!dir.exists()) if(!dir.mkpath(dir.path())) dir.setPath("");
		if(!dir.isReadable()) dir.setPath("");
		if(!dir.path().isEmpty()) settings.setNotesPath(dir.path());
	}
#endif
	while(dir.path().isEmpty())
	{
		dir.setPath(QFileDialog::getExistingDirectory(0,
			QObject::tr("Select notes directory"), QDir::homePath(),
			QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks));
		if(!dir.path().isEmpty()) settings.setNotesPath(dir.path());
	}
	if(!dir.exists()) dir.mkpath(dir.path());
	dir.setFilter(QDir::Files);
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
	bool v = isVisible();
	Qt::WindowFlags flags = Qt::Window;
	if(settings.getHideFrame()) flags |= Qt::FramelessWindowHint;
	if(settings.getStayTop()) flags |= Qt::WindowStaysOnTopHint;
	setWindowFlags(flags);
	if(v) show();
}

void MainWindow::toolbarVisChanged()
{
	if(settings.getHideToolbar()) ui->mainToolBar->hide();
	else ui->mainToolBar->show();
}

void MainWindow::noteFontChanged()
{
	for(int i=0; i<Notes.count(); ++i)
	{
		Notes[i]->setFont(settings.getNoteFont());
	}
}

void MainWindow::commandMenu()
{
	QPoint p = ui->mainToolBar->actionGeometry(actRun).center();
	p+=pos();
	cmd_menu.exec(p);
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
	if(!settings.getTbHideEdit())
	{
		ui->mainToolBar->addAction(actAdd);
		ui->mainToolBar->addAction(actRemove);
		ui->mainToolBar->addAction(actRename);
		ui->mainToolBar->addSeparator();
	}
	if(!settings.getTbHideMove())
	{
		ui->mainToolBar->addAction(actPrev);
		ui->mainToolBar->addAction(actNext);
		ui->mainToolBar->addSeparator();
	}
	if(!settings.getTbHideCopy())
	{
		ui->mainToolBar->addAction(actCopy);
		ui->mainToolBar->addSeparator();
	}
	if(!settings.getTbHideSetup())
	{
		ui->mainToolBar->addAction(actSetup);
		ui->mainToolBar->addAction(actInfo);
		ui->mainToolBar->addSeparator();
	}
	if(!settings.getTbHideRun())
	{
		ui->mainToolBar->addAction(actRun);
		ui->mainToolBar->addAction(actSearch);
		ui->mainToolBar->addSeparator();
	}
	if(!settings.getTbHideExit()) ui->mainToolBar->addAction(actExit);
}

MainWindow::MainWindow(QWidget *parent)
	: QMainWindow(parent), ui(new Ui::MainWindow), CurrentIndex(-1)
{
	ui->setupUi(this);
	ui->wSearch->hide();
	//
	restoreGeometry(settings.getDialogGeometry());
	restoreState(settings.getDialogState());
	windowStateChanged();
	//
	actAdd = new QAction(QIcon(":/res/add.png"), tr("Create new note"), parent);
	actRemove = new QAction(QIcon(":/res/remove.png"), tr("Remove this note"), parent);
	actRename = new QAction(QIcon(":/res/rename.png"), tr("Rename this note"), parent);
	actPrev = new QAction(QIcon(":/res/prev.png"), tr("Previous note"), parent);
	actNext = new QAction(QIcon(":/res/next.png"), tr("Next note"), parent);
	actCopy = new QAction(QIcon(":/res/copy.png"), tr("Copy this note to clipboard"), parent);
	actSetup = new QAction(QIcon(":/res/settings.png"), tr("Preferences"), parent);
	actInfo = new QAction(QIcon(":/res/info.png"), tr("Info"), parent);
	actRun = new QAction(QIcon(":/res/exec.png"), tr("Commands"), parent);
	actSearch = new QAction(QIcon(":/res/find.png"), tr("Search"), parent);
	actExit = new QAction(QIcon(":/res/exit.png"), tr("Exit"), parent);
	//
	QObject::connect(actAdd, SIGNAL(triggered()), this, SLOT(NewNote()));
	QObject::connect(actRemove, SIGNAL(triggered()), this, SLOT(RemoveCurrentNote()));
	QObject::connect(actRename, SIGNAL(triggered()), this, SLOT(RenameCurrentNote()));
	QObject::connect(actPrev, SIGNAL(triggered()), this, SLOT(PreviousNote()));
	QObject::connect(actNext, SIGNAL(triggered()), this, SLOT(NextNote()));
	QObject::connect(actCopy, SIGNAL(triggered()), this, SLOT(CopyNote()));
	QObject::connect(actSetup, SIGNAL(triggered()), this, SLOT(showPrefDialog()));
	QObject::connect(actInfo, SIGNAL(triggered()), this, SLOT(showAboutDialog()));
	QObject::connect(actRun, SIGNAL(triggered()), this, SLOT(commandMenu()));
	QObject::connect(actSearch, SIGNAL(triggered()), this, SLOT(showSearchBar()));
	QObject::connect(actExit, SIGNAL(triggered()), qApp, SLOT(quit()));
	//
	actions_changed();
	cmd_changed();
	//
	cmenu.addAction(tr("Show"), this, SLOT(show()));
	cmenu.addAction(tr("Hide"), this, SLOT(hide()));
	cmenu.addSeparator();
	cmenu.addAction(QIcon(":/res/add.png"), tr("Create new note"), this, SLOT(NewNote()));
	cmenu.addAction(QIcon(":/res/remove.png"), tr("Remove this note"), this, SLOT(RemoveCurrentNote()));
	cmenu.addAction(QIcon(":/res/rename.png"), tr("Rename this note"), this, SLOT(RenameCurrentNote()));
	cmenu.addSeparator();
	cmenu.addAction(QIcon(":/res/settings.png"), tr("Preferences"), this, SLOT(showPrefDialog()));
	cmenu.addAction(QIcon(":/res/info.png"), tr("About"), this, SLOT(showAboutDialog()));
	cmenu.addSeparator();
	cmenu.addAction(QIcon(":/res/exit.png"), tr("Quit"), qApp, SLOT(quit()));
	tray.setIcon(QIcon(":/res/znotes32.png"));
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
	LoadNotes();
	if(Notes.count()==0) NewNote();
	if(Notes.count()<2)
	{
		ui->mainToolBar->actions()[1]->setDisabled(true);
		cmenu.actions()[4]->setDisabled(true);
	}
	//
	connect(&SaveTimer, SIGNAL(timeout()), this, SLOT(SaveAll()));
	SaveTimer.start(100000);
	//
	connect(&settings, SIGNAL(NotesPathChanged()), this, SLOT(notesPathChanged()));
	connect(&settings, SIGNAL(WindowStateChanged()), this, SLOT(windowStateChanged()));
	connect(&settings, SIGNAL(ToolbarVisChanged()), this, SLOT(toolbarVisChanged()));
	connect(&settings, SIGNAL(NoteFontChanged()), this, SLOT(noteFontChanged()));
	connect(&settings, SIGNAL(tbHidingChanged()), this, SLOT(actions_changed()));
	connect(&settings.getScriptModel(), SIGNAL(dataChanged(const QModelIndex &, const QModelIndex &)),
		this, SLOT(cmd_changed()));
	if(!settings.getHideStart()) show();
}

MainWindow::~MainWindow()
{
	delete ui;
	SaveAll();
	settings.setLastNote(currentNote()->name);
	settings.setDialogGeometry(saveGeometry());
	settings.setDialogState(saveState());
	//
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

void MainWindow::on_edSearch_textChanged(QString text)
{
	//TODO: шде-то тут косяк
	if(text.isEmpty()) return;
	int start_index = CurrentIndex;
	for(int i=start_index; i<Notes.size(); ++i)
	{
		if(Notes[i]->find(text))
		{
			ui->tabs->setCurrentIndex(i);
			return;
		}
		//qDebug() << i;
	}
		//qDebug() << "ololo";
	for(int i=0; i<start_index; ++i)
	{
		if(Notes[i]->find(text))
		{
			ui->tabs->setCurrentIndex(i);
			return;
		}
		//qDebug() << i;
	}
}

void MainWindow::on_edSearch_returnPressed()
{
	on_edSearch_textChanged(ui->edSearch->text());
}
