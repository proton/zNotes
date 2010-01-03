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
#include <QCloseEvent>

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
	if(Notes.count()==0) return;
	Note* note = currentNote();
	QMessageBox msgBox(QMessageBox::Question, tr("Delete Note"),
		tr("Do you realy want to delete note %1 ?").arg(note->title()),
		QMessageBox::Yes | QMessageBox::No);
	int ret = msgBox.exec();
	if(ret == QMessageBox::Yes && note->remove())
	{
		const int current_index = ui->tabs->currentIndex();
		ui->tabs->removeTab(current_index);
		Notes.remove(current_index);
		delete note;
		if(Notes.count()==0)
		{
			actRemove->setDisabled(true);
			actRename->setDisabled(true);
		}
	}
}

void MainWindow::RenameCurrentNote()
{
	if(Notes.count()==0) return;
	//
	Note* note = currentNote();
	note->save();
	bool ok;
	QString new_name = QInputDialog::getText(this, tr("Rename note"), tr("New name:"), QLineEdit::Normal, note->title(), &ok);
	if(ok && !new_name.isEmpty())
	{
		note->rename(new_name);
		ui->tabs->setTabText(ui->tabs->currentIndex(), new_name);
	}
}

void MainWindow::NewNote()
{
	int n = 0;
	QString filename = QString::number(n);
	QFile file(dir.absoluteFilePath(filename));
	while(file.exists())
	{
		filename = QString::number(++n);
		file.setFileName(dir.absoluteFilePath(filename));
	}
	Note* note = new Note(file);
	Notes.append(note);
	ui->tabs->addTab(note->widget(), note->title());
	ui->tabs->setCurrentWidget(note->widget());
	if(Notes.count()>0)
	{
		actRemove->setEnabled(true);
		actRename->setEnabled(true);
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
	currentNote()->copy();
}

void MainWindow::LoadNotes()
{
	ui->tabs->clear(); //Clearing tabs
	//Setting directory
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
	//Loading files' list
	dir.setFilter(QDir::Files | QDir::Readable);
	QFileInfoList flist = dir.entryInfoList();
	const QString& old_note = settings.getLastNote();
	int old_index=-1;
	for(int i=0; i<flist.size(); ++i)
	{
		//Loading note
		Note* note = new Note(flist.at(i));
		Notes.append(note);
		ui->tabs->addTab(note->widget(), note->title());
		if(old_index==-1 && note->title()==old_note) old_index = i;
	}
	if(old_index!=-1) ui->tabs->setCurrentIndex(old_index);
}

void MainWindow::SaveAll()
{
	for(int i=0; i<Notes.size(); ++i) Notes[i]->save();
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
	actShow->setEnabled(true);
	actHide->setDisabled(true);
	settings.setDialogGeometry(saveGeometry());
	SaveAll();
}

void MainWindow::showEvent(QShowEvent */*event*/)
{
	actShow->setEnabled(false);
	actHide->setDisabled(false);
	restoreGeometry(settings.getDialogGeometry());
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
		QString dir_path = dir.absolutePath();
		for(int i=0; i<Notes.count(); ++i) Notes[i]->move(dir_path);
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
	args << currentNote()->absolutePath();
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
	//Creating toolbar/menu actions
	actAdd	=	new QAction(ToolbarAction(itemAdd).icon(),		ToolbarAction(itemAdd).text(),		parent);
	actRemove =	new QAction(ToolbarAction(itemRemove).icon(),	ToolbarAction(itemRemove).text(),	parent);
	actRename =	new QAction(ToolbarAction(itemRename).icon(),	ToolbarAction(itemRename).text(),	parent);
	actPrev =	new QAction(ToolbarAction(itemPrev).icon(),		ToolbarAction(itemPrev).text(),		parent);
	actNext =	new QAction(ToolbarAction(itemNext).icon(),		ToolbarAction(itemNext).text(),		parent);
	actCopy =	new QAction(ToolbarAction(itemCopy).icon(),		ToolbarAction(itemCopy).text(),		parent);
	actSetup =	new QAction(ToolbarAction(itemSetup).icon(),	ToolbarAction(itemSetup).text(),	parent);
	actInfo =	new QAction(ToolbarAction(itemInfo).icon(),		ToolbarAction(itemInfo).text(),		parent);
	actRun	=	new QAction(ToolbarAction(itemRun).icon(),		ToolbarAction(itemRun).text(),		parent);
	actSearch =	new QAction(ToolbarAction(itemSearch).icon(),	ToolbarAction(itemSearch).text(),	parent);
	actExit =	new QAction(ToolbarAction(itemExit).icon(),		ToolbarAction(itemExit).text(),		parent);
	actFormatBold =	   new QAction(ToolbarAction(itemFormatBold).icon(),	  ToolbarAction(itemFormatBold).text(),		parent);
	actFormatStrikeout=new QAction(ToolbarAction(itemFormatStrikeout).icon(), ToolbarAction(itemFormatStrikeout).text(),parent);
	actFormatUnderline=new QAction(ToolbarAction(itemFormatUnderline).icon(), ToolbarAction(itemFormatUnderline).text(),parent);
	actShow =	new QAction(tr("Show"),	parent);
	actHide =	new QAction(tr("Hide"),	parent);
	//Connecting actions with slots
	connect(actAdd,		SIGNAL(triggered()), this, SLOT(NewNote()));
	connect(actRemove,	SIGNAL(triggered()), this, SLOT(RemoveCurrentNote()));
	connect(actRename,	SIGNAL(triggered()), this, SLOT(RenameCurrentNote()));
	connect(actPrev,	SIGNAL(triggered()), this, SLOT(PreviousNote()));
	connect(actNext,	SIGNAL(triggered()), this, SLOT(NextNote()));
	connect(actCopy,	SIGNAL(triggered()), this, SLOT(CopyNote()));
	connect(actSetup,	SIGNAL(triggered()), this, SLOT(showPrefDialog()));
	connect(actInfo,	SIGNAL(triggered()), this, SLOT(showAboutDialog()));
	connect(actRun,		SIGNAL(triggered()), this, SLOT(commandMenu()));
	connect(actSearch,	SIGNAL(triggered()), this, SLOT(showSearchBar()));
	connect(actExit,	SIGNAL(triggered()), qApp, SLOT(quit()));
	connect(actShow,	SIGNAL(triggered()), this, SLOT(show()));
	connect(actHide,	SIGNAL(triggered()), this, SLOT(hide()));
	//
	actions_changed(); //Adding toolbar's actions
	cmd_changed(); //Adding scripts
	//Adding menu actions
	cmenu.addAction(actShow);
	cmenu.addAction(actHide);
	cmenu.addSeparator();
	cmenu.addAction(actAdd);
	cmenu.addAction(actRemove);
	cmenu.addAction(actRename);
	cmenu.addSeparator();
	cmenu.addAction(actSetup);
	cmenu.addAction(actInfo);
	cmenu.addSeparator();
	cmenu.addAction(actExit);
	tray.setIcon(QIcon(TRAY_ICON_FILE_NAME));
	tray.setContextMenu(&cmenu);
	connect(&tray, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
			this, SLOT(trayActivated(QSystemTrayIcon::ActivationReason)));
	tray.show();
	//Creating shortcuts
	scAdd	=	new QShortcut(QKeySequence::New,	this);
	scRemove =	new QShortcut(QKeySequence::Delete,	this);
	scPrev =	new QShortcut(QKeySequence::Back,	this);
	scNext =	new QShortcut(QKeySequence::Forward,this);
	scSearch =	new QShortcut(QKeySequence::Find,	this);
	scExit =	new QShortcut(QKeySequence::Close,	this);
	//Connecting shortcuts with slots
	connect(scAdd,		SIGNAL(activated()), this, SLOT(NewNote()));
	connect(scRemove,	SIGNAL(activated()), this, SLOT(RemoveCurrentNote()));
	connect(scPrev,		SIGNAL(activated()), this, SLOT(PreviousNote()));
	connect(scNext,		SIGNAL(activated()), this, SLOT(NextNote()));
	connect(scSearch,	SIGNAL(activated()), this, SLOT(showSearchBar()));
	connect(scExit,		SIGNAL(activated()), qApp, SLOT(quit()));
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
	//
	connect(&SaveTimer, SIGNAL(timeout()), this, SLOT(SaveAll()));
	SaveTimer.start(10000);
	//
	connect(&settings, SIGNAL(NotesPathChanged()), this, SLOT(notesPathChanged()));
	connect(&settings, SIGNAL(WindowStateChanged()), this, SLOT(windowStateChanged()));
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
	settings.setLastNote(currentNote()->title());
	//saving dialog's params
	settings.setDialogGeometry(saveGeometry());
	settings.setDialogState(saveState());
	//saving scrits
	settings.setScripts();
}

void MainWindow::on_tabs_currentChanged(int index)
{
	if(index==-1) return;
	if(CurrentIndex!=-1) currentNote()->save();
	CurrentIndex = index;
	actPrev->setDisabled(index==0); //first note
	actNext->setDisabled(index==Notes.count()-1); //last note
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
	Q_UNUSED(next);
	if(Notes.count()==0) return;
	QString text = ui->edSearch->text();
	if(text.isEmpty()) return;
	//Searching in current note
	if(currentNote()->find(text)) return;
	//Searching in all notes
	const int max = Notes.count()+CurrentIndex;
	for(int n=CurrentIndex+1; n<=max; ++n)
	{
		int i = n%Notes.count(); //secret formula of success search
		if(Notes[i]->find(text, true))
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
		actShow->setText(tr("Show"));
		actHide->setText(tr("Hide"));
		//
		break;
	default: break;
	}
}

