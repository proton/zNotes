#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "settings.h"
#include "configdialog.h"

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
	while(dir.path().isEmpty())
	{
		dir.setPath(QFileDialog::getExistingDirectory(0,
			QObject::tr("Select notes directory"), QDir::homePath(),
			QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks));
		if(!dir.path().isEmpty()) settings.setNotesPath(dir.path());
	}
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

void MainWindow::aboutDialog()
{
	QMessageBox::information(this, tr("zNotes - about"),
		tr("zNotes %1\nby Peter Savichev (proton)\npsavichev@gmail.com\n2009").arg(VERSION));
}

void MainWindow::prefDialog()
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
	QPoint p = ui->mainToolBar->actionGeometry(&cmd_action).center();
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

MainWindow::MainWindow(QWidget *parent)
	: QMainWindow(parent), ui(new Ui::MainWindow), CurrentIndex(-1), cmd_action(QIcon(":/res/exec.png"), tr("Commands"), parent)
{
	ui->setupUi(this);
	//
	restoreGeometry(settings.getDialogGeometry());
	windowStateChanged();
	//
	ui->mainToolBar->addAction(QIcon(":/res/add.png"), tr("Create new note"),
							   this, SLOT(NewNote()));
	ui->mainToolBar->addAction(QIcon(":/res/remove.png"), tr("Remove this note"),
							   this, SLOT(RemoveCurrentNote()));
	ui->mainToolBar->addAction(QIcon(":/res/rename.png"), tr("Rename this note"),
							   this, SLOT(RenameCurrentNote()));
	ui->mainToolBar->addSeparator();
	ui->mainToolBar->addAction(QIcon(":/res/prev.png"), tr("Previous note"),
							   this, SLOT(PreviousNote()));
	ui->mainToolBar->addAction(QIcon(":/res/next.png"), tr("Next note"),
							   this, SLOT(NextNote()));
	ui->mainToolBar->addSeparator();
	ui->mainToolBar->addAction(QIcon(":/res/copy.png"), tr("Copy this note to clipboard"),
							   this, SLOT(CopyNote()));
	ui->mainToolBar->addSeparator();
	ui->mainToolBar->addAction(QIcon(":/res/settings.png"), tr("Preferences"),
							   this, SLOT(prefDialog()));
	ui->mainToolBar->addSeparator();
	ui->mainToolBar->addAction(&cmd_action);
	connect(&cmd_action, SIGNAL(triggered()), this, SLOT(commandMenu()));
	ui->mainToolBar->actions()[0]->setShortcut(QKeySequence::New);
	ui->mainToolBar->actions()[1]->setShortcut(QKeySequence::Delete);
	//
	cmd_changed();
	//
	cmenu.addAction(tr("Show"), this, SLOT(show()));
	cmenu.addAction(tr("Hide"), this, SLOT(hide()));
	cmenu.addSeparator();
	cmenu.addAction(QIcon(":/res/add.png"), tr("Create new note"), this, SLOT(NewNote()));
	cmenu.addAction(QIcon(":/res/remove.png"), tr("Remove this note"), this, SLOT(RemoveCurrentNote()));
	cmenu.addAction(QIcon(":/res/rename.png"), tr("Rename this note"), this, SLOT(RenameCurrentNote()));
	cmenu.addSeparator();
	cmenu.addAction(QIcon(":/res/settings.png"), tr("Preferences"), this, SLOT(prefDialog()));
	cmenu.addAction(QIcon(":/res/info.png"), tr("About"), this, SLOT(aboutDialog()));
	cmenu.addSeparator();
	cmenu.addAction(QIcon(":/res/exit.png"), tr("Quit"), qApp, SLOT(quit()));
	cmenu.actions()[10]->setShortcut(QKeySequence::Close);
	tray.setIcon(QIcon(":/res/znotes32.png"));
	tray.setContextMenu(&cmenu);
	connect(&tray, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
			this, SLOT(trayActivated(QSystemTrayIcon::ActivationReason)));
	tray.show();
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
	settings.setScripts();
}

void MainWindow::on_tabs_currentChanged(int index)
{
	SaveNote(CurrentIndex);
	CurrentIndex = index;
	ui->mainToolBar->actions()[4]->setDisabled(index==0);
	ui->mainToolBar->actions()[5]->setDisabled(index==Notes.count()-1);
}
