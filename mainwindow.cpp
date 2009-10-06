#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "settings.h"
#include "configdialog.h"

#include <QtDebug>
#include <QInputDialog>
#include <QMessageBox>
#include <QFileDialog>

Settings settings;

Note::Note(const QString& fn, const QDir& dir) : QPlainTextEdit(), name(fn), file(dir.absoluteFilePath(fn))
{
	if(file.open(QIODevice::ReadOnly | QIODevice::Text))
	{
		QTextStream in(&file);
		setPlainText(in.readAll());
		file.close();
	}
	else if(file.open(QIODevice::WriteOnly | QIODevice::Text)) file.close();
}

Note* MainWindow::currentNote()
{
	return Notes[CurrentIndex];
}

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
	}
}

void MainWindow::RenameCurrentNote()
{
	Note* n = currentNote();
	bool ok;
	QString text =
		QInputDialog::getText(this,
			tr("Rename note"), tr("New name:"),
			QLineEdit::Normal, n->name, &ok);
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
	SaveNote(currentIndex);
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
	Notes.append(new Note(fn, dir));
	ui->tabs->addTab(Notes[index], fn);
	ui->tabs->setCurrentIndex(index);
	QObject::connect(Notes[index], SIGNAL(textChanged()), this, SLOT(currentNoteChanged()));
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
		Notes[i] = new Note(flist.at(i).fileName(), dir);
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
	SaveAll();
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
	QMessageBox::information(this, tr("zNotes"),
		tr("zNotes\nby Peter Savichev (proton)\npsavichev@gmail.com\n2009"));
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
		tr("You need restart application to get effect"));
}

MainWindow::MainWindow(QWidget *parent)
	: QMainWindow(parent), ui(new Ui::MainWindow), CurrentIndex(-1)
{
	ui->setupUi(this);
	//
	setGeometry(settings.getDialogRect());
	move(settings.getDialogPos());
	//
	LoadNotes();
	if(Notes.count()==0) NewNote();
	//
	ui->mainToolBar->addAction(QIcon(":/res/add.png"), tr("Create new note"),
							   this, SLOT(NewNote()));
	ui->mainToolBar->addAction(QIcon(":/res/remove.png"), tr("Remove this note"),
							   this, SLOT(RemoveCurrentNote()));
	ui->mainToolBar->addAction(QIcon(":/res/rename.png"), tr("Rename this note"),
							   this, SLOT(RenameCurrentNote()));
	ui->mainToolBar->addSeparator();
	ui->mainToolBar->addAction(QIcon(":/res/settings.png"), tr("Preferences"),
							   this, SLOT(prefDialog()));
	ui->mainToolBar->actions()[0]->setShortcut(QKeySequence::New);
	ui->mainToolBar->actions()[1]->setShortcut(QKeySequence::Delete);
	//
	cmenu.addAction(tr("Show"), this, SLOT(show()));
	cmenu.addAction(tr("Hide"), this, SLOT(hide()));
	cmenu.addSeparator();
	cmenu.addAction(QIcon(":/res/settings.png"), tr("Preferences"), this, SLOT(prefDialog()));
	cmenu.addAction(QIcon(":/res/info.png"), tr("About"), this, SLOT(aboutDialog()));
	cmenu.addSeparator();
	cmenu.addAction(QIcon(":/res/exit.png"), tr("Quit"), qApp, SLOT(quit()));
	cmenu.actions()[6]->setShortcut(QKeySequence::Close);
	tray.setIcon(QIcon(":/res/znotes32.png"));
	tray.setContextMenu(&cmenu);
	connect(&tray, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
			this, SLOT(trayActivated(QSystemTrayIcon::ActivationReason)));
	tray.show();
	//
	connect(&SaveTimer, SIGNAL(timeout()), this, SLOT(SaveAll()));
	SaveTimer.start(100000);
	//
	connect(&settings, SIGNAL(NotesPathChanged()), this, SLOT(notesPathChanged()));
	if(!settings.getHideStart()) show();
}

MainWindow::~MainWindow()
{
	delete ui;
	SaveAll();
	settings.setLastNote(currentNote()->name);
	settings.setDialogRect(rect());
	settings.setDialogPos(pos());
}

void MainWindow::on_tabs_currentChanged(int index)
{
	SaveNote(CurrentIndex);
	CurrentIndex = index;
}
