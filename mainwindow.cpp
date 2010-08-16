#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "settings.h"
#include "configdialog.h"
#include "aboutDialog.h"
#include "note_html.h"

#include <QInputDialog>
#include <QMessageBox>
#include <QFileDialog>
#include <QClipboard>
#include <QProcess>
#include <QCloseEvent>
#include <QTextCharFormat>
#include <QColorDialog>

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
	if(Notes->empty()) return;
	Note* note = Notes->current();
	QMessageBox msgBox(QMessageBox::Question, tr("Delete Note"),
		tr("Do you realy want to delete note %1 ?").arg(note->title()),
		QMessageBox::Yes | QMessageBox::No);
	int ret = msgBox.exec();
	if(ret == QMessageBox::Yes && note->remove())
	{
		Notes->remove(Notes->currentIndex());
		if(Notes->empty())
		{
			for(int i=0; i<itemMax; ++i)
			{
				actions[i]->setDisabled(!ToolbarAction(item_enum(i)).isEnabledWhenEmpty());
			}
		}
	}
}

void MainWindow::RenameCurrentNote()
{
	if(Notes->empty()) return;
	//
	QString filename = Notes->current()->fileName();
	bool ok;
	QString new_name = QInputDialog::getText(this, tr("Rename note"), tr("New name:"), QLineEdit::Normal, filename, &ok);
	if(ok && !new_name.isEmpty())
	{
		QFile file(dir.absoluteFilePath(filename));
		if(!file.exists())
			Notes->rename(Notes->currentIndex(), new_name);
		else
			QMessageBox::information(this, tr("Note renaming"),
				tr("Note %1 already exists!").arg(new_name));
	}
}

void MainWindow::NewNote(const QString& mask)
{
	int n = 0;
	QString filename = QString(mask).arg(n);
	QFile file(dir.absoluteFilePath(filename));
	while(file.exists()) //Searching for free filename
	{
		filename = QString(mask).arg(++n);
		file.setFileName(dir.absoluteFilePath(filename));
	}
	if(Notes->empty())
	{
		for(int i=0; i<itemMax ; ++i)
		{
			actions[i]->setEnabled(true);
		}
	}
	Notes->add(file);
}

void MainWindow::NewNotePlain()
{
	NewNote("%1");
}

void MainWindow::NewNoteHTML()
{
	NewNote("%1.htm");
}

void MainWindow::NewNoteTODO()
{
	NewNote("%1.ztodo");
}

void MainWindow::PreviousNote()
{
	if(Notes->empty()) return;
	Notes->getWidget()->setCurrentIndex(Notes->currentIndex()-1);
}

void MainWindow::NextNote()
{
	if(Notes->empty()) return;
	Notes->getWidget()->setCurrentIndex(Notes->currentIndex()+1);
}

void MainWindow::ToNote(int n)
{
	if(n>=Notes->count()) return;
	Notes->getWidget()->setCurrentIndex(n);
}

void MainWindow::CopyNote()
{
	if(Notes->empty()) return;
	Notes->current()->copy();
}

void MainWindow::LoadNotes()
{
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
	if(settings.getShowHidden()) dir.setFilter(QDir::Files | QDir::Hidden | QDir::Readable);
	else dir.setFilter(QDir::Files | QDir::Readable);
	QFileInfoList flist = dir.entryInfoList();
	const QString& old_note = settings.getLastNote();
	int old_index=-1;
	for(int i=0; i<flist.size(); ++i)
	{
		//Loading note
		bool is_old = Notes->load(flist.at(i), old_note);
		if(is_old) old_index = i;
	}
	if(old_index!=-1) Notes->setCurrent(old_index);
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
	settings.setDialogGeometry(saveGeometry());
	actShow->setEnabled(true);
	actHide->setDisabled(true);
	if(Notes->current()) Notes->SaveAll();
}

void MainWindow::showEvent(QShowEvent */*event*/)
{
	restoreGeometry(settings.getDialogGeometry());
	actShow->setEnabled(false);
	actHide->setDisabled(false);
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
		Notes->move(dir_path);
	}
	else QMessageBox::information(this, tr("Notes path change"),
		tr("You need restart application to get effect."));
}

void MainWindow::fileScannerEnChanged(bool enabled)
{
	if(enabled) ScanTimer.start();
	else ScanTimer.stop();
}

void MainWindow::fileScannerTimeoutChanged(int period)
{
	ScanTimer.setInterval(period);
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

void MainWindow::scanForNewFiles()
{
	if(settings.getShowHidden()) dir.setFilter(QDir::Files | QDir::Hidden | QDir::Readable);
	else dir.setFilter(QDir::Files | QDir::Readable);
	dir.refresh();
	QFileInfoList flist = dir.entryInfoList();
	for(int i=0; i<flist.size(); ++i)
	{
		//Loading note
		bool exists = Notes->has(flist.at(i).fileName());
		if(!exists) Notes->add(flist.at(i));
	}
}

void MainWindow::warningSettingsChanged()
{
	QMessageBox::information(this, tr("Settings changed"),
			tr("You need restart application to get effect."));
}

void MainWindow::commandMenu()
{
	if(cmd_menu.actions().size()>0)
	{
		QPoint p = ui->mainToolBar->actionGeometry(actions[itemRun]).center()+pos();
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
	args << Notes->current()->absolutePath();
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
		else ui->mainToolBar->addAction(actions[items[i]]);
	}
}

void MainWindow::formatChanged(const QFont& font)
{
	actions[itemFormatBold]->setChecked(font.bold());
	actions[itemFormatItalic]->setChecked(font.italic());
	actions[itemFormatStrikeout]->setChecked(font.strikeOut());
	actions[itemFormatUnderline]->setChecked(font.underline());
}

void MainWindow::formatBold()
{
	if(Notes->empty()) return;
	if(Notes->current()->type()!=Note::type_html) return;
	QTextCharFormat format;
	bool is_bold = actions[itemFormatBold]->isChecked();
	format.setFontWeight(is_bold?QFont::Bold : QFont::Normal);
	Notes->current()->setSelFormat(format);
}

void MainWindow::formatItalic()
{
	if(Notes->empty()) return;
	if(Notes->current()->type()!=Note::type_html) return;
	QTextCharFormat format;
	bool is_italic = actions[itemFormatItalic]->isChecked();
	format.setFontItalic(is_italic);
	Notes->current()->setSelFormat(format);
}

void MainWindow::formatStrikeout()
{
	if(Notes->empty()) return;
	if(Notes->current()->type()!=Note::type_html) return;
	QTextCharFormat format;
	bool is_strikeout = actions[itemFormatStrikeout]->isChecked();
	format.setFontStrikeOut(is_strikeout);
	Notes->current()->setSelFormat(format);
}

void MainWindow::formatUnderline()
{
	if(Notes->empty()) return;
	if(Notes->current()->type()!=Note::type_html) return;
	QTextCharFormat format;
	bool is_underline = actions[itemFormatUnderline]->isChecked();
	format.setFontUnderline(is_underline);
	Notes->current()->setSelFormat(format);
}

void MainWindow::formatTextColor()
{
	if(Notes->empty()) return;
	Note* note = Notes->current();
	if(note->type()!=Note::type_html) return;
	QTextCharFormat format = note->getSelFormat();
	QColor color = format.foreground().color();
	QColorDialog dlg(color);
	if(dlg.exec()==QDialog::Accepted)
	{
		color = dlg.currentColor();
		format.setForeground(color);
		note->setSelFormat(format);
	}
}

//------------------------------------------------------------------------------

//Function for fast generating actions
inline QAction* GenerateAction(int item_id /*, bool checkable = false*/)
{
	item_enum item = item_enum(item_id);
	ToolbarAction toolbar_action(item);
	QAction* action = new QAction(toolbar_action.icon(), toolbar_action.text(), 0);
	action->setCheckable(toolbar_action.isCheckable());
	return action;
}

MainWindow::MainWindow(QWidget *parent)
	: QMainWindow(parent), ui(new Ui::MainWindow)
{
	ui->setupUi(this);
	Notes = new NoteList(ui->centralWidget);
	ui->layout->addWidget(Notes->getWidget());
	connect(Notes, SIGNAL(currentNoteChanged(int,int)), this, SLOT(currentNoteChanged(int,int)));
	ui->wSearch->hide();
	//restoring window state
	restoreGeometry(settings.getDialogGeometry());
	restoreState(settings.getDialogState());
	windowStateChanged();
	//Creating toolbar/menu actions
	for(int i=0; i<itemMax ; ++i) actions[i] = GenerateAction(i);
	actShow =	new QAction(tr("Show"),	parent);
	actHide =	new QAction(tr("Hide"),	parent);
	//Connecting actions with slots
	connect(actions[itemAdd],		SIGNAL(triggered()), this, SLOT(NewNotePlain()));
	connect(actions[itemAddHtml],	SIGNAL(triggered()), this, SLOT(NewNoteHTML()));
	connect(actions[itemAddTodo],	SIGNAL(triggered()), this, SLOT(NewNoteTODO()));
	connect(actions[itemRemove],	SIGNAL(triggered()), this, SLOT(RemoveCurrentNote()));
	connect(actions[itemRename],	SIGNAL(triggered()), this, SLOT(RenameCurrentNote()));
	connect(actions[itemPrev],	SIGNAL(triggered()), this, SLOT(PreviousNote()));
	connect(actions[itemNext],	SIGNAL(triggered()), this, SLOT(NextNote()));
	connect(actions[itemBack],	SIGNAL(triggered()), Notes, SLOT(historyBack()));
	connect(actions[itemForward],	SIGNAL(triggered()), Notes, SLOT(historyForward()));
	connect(actions[itemCopy],	SIGNAL(triggered()), this, SLOT(CopyNote()));
	connect(actions[itemSetup],	SIGNAL(triggered()), this, SLOT(showPrefDialog()));
	connect(actions[itemInfo],	SIGNAL(triggered()), this, SLOT(showAboutDialog()));
	connect(actions[itemRun],		SIGNAL(triggered()), this, SLOT(commandMenu()));
	connect(actions[itemSearch],	SIGNAL(toggled(bool)), this, SLOT(showSearchBar(bool)));
	connect(actions[itemExit],	SIGNAL(triggered()), qApp, SLOT(quit()));
	connect(actions[itemFormatBold],	SIGNAL(triggered()), this, SLOT(formatBold()));
	connect(actions[itemFormatItalic],	SIGNAL(triggered()), this, SLOT(formatItalic()));
	connect(actions[itemFormatStrikeout],	SIGNAL(triggered()), this, SLOT(formatStrikeout()));
	connect(actions[itemFormatUnderline],	SIGNAL(triggered()), this, SLOT(formatUnderline()));
	connect(actions[itemFormatColor],	SIGNAL(triggered()), this, SLOT(formatTextColor()));
	//
	connect(actShow,	SIGNAL(triggered()), this, SLOT(show()));
	connect(actHide,	SIGNAL(triggered()), this, SLOT(hide()));
	//
	actions_changed(); //Adding toolbar's actions
	cmd_changed(); //Adding scripts
	//Adding menu actions
	cmenu.addAction(actShow);
	cmenu.addAction(actHide);
	cmenu.addSeparator();
	cmenu.addAction(actions[itemAdd]);
	cmenu.addAction(actions[itemRemove]);
	cmenu.addAction(actions[itemRename]);
	cmenu.addSeparator();
	cmenu.addAction(actions[itemSetup]);
	cmenu.addAction(actions[itemInfo]);
	cmenu.addSeparator();
	cmenu.addAction(actions[itemExit]);
	tray.setIcon(QIcon(TRAY_ICON_FILE_NAME));
	tray.setContextMenu(&cmenu);
	connect(&tray, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
			this, SLOT(trayActivated(QSystemTrayIcon::ActivationReason)));
	tray.show();
	//Creating shortcuts
	scAdd	=	new QShortcut(QKeySequence::New,	this);
	scRemove =	new QShortcut(QKeySequence::Delete,	this);
	scRename =	new QShortcut(Qt::Key_F2,	this);
	scBack =	new QShortcut(QKeySequence::Back,	this);
	scForward =	new QShortcut(QKeySequence::Forward,this);
	scPrev =	new QShortcut(QKeySequence::PreviousChild,	this);
	scNext =	new QShortcut(QKeySequence::NextChild,this);
	scSearch =	new QShortcut(QKeySequence::Find,	this);
	scSearchEsc = new QShortcut(Qt::Key_Escape, ui->edSearch, 0, 0, Qt::WidgetShortcut);
	scExit =	new QShortcut(QKeySequence::Close,	this);
	scFormatBold =		new QShortcut(Qt::CTRL + Qt::Key_B,	this);
	scFormatItalic =	new QShortcut(Qt::CTRL + Qt::Key_I,	this);
	scFormatStrikeout = new QShortcut(Qt::CTRL + Qt::Key_S,	this);
	scFormatUnderline = new QShortcut(Qt::CTRL + Qt::Key_U,	this);
	//Connecting shortcuts with slots
	connect(scAdd,		SIGNAL(activated()), this, SLOT(NewNotePlain()));
	connect(scRemove,	SIGNAL(activated()), this, SLOT(RemoveCurrentNote()));
	connect(scRename,	SIGNAL(activated()), this, SLOT(RenameCurrentNote()));
	connect(scPrev,		SIGNAL(activated()), this, SLOT(PreviousNote()));
	connect(scNext,		SIGNAL(activated()), this, SLOT(NextNote()));
	connect(scBack,		SIGNAL(activated()), Notes, SLOT(historyBack()));
	connect(scForward,	SIGNAL(activated()), Notes, SLOT(historyForward()));
	connect(scSearch,	SIGNAL(activated()), actions[itemSearch], SLOT(toggle()));
	connect(scSearchEsc,	SIGNAL(activated()), actions[itemSearch], SLOT(toggle()));
	connect(scExit,		SIGNAL(activated()), qApp, SLOT(quit()));
	connect(scFormatBold,	SIGNAL(activated()), this, SLOT(formatBold()));
	connect(scFormatItalic,	SIGNAL(activated()), this, SLOT(formatItalic()));
	connect(scFormatStrikeout,	SIGNAL(activated()), this, SLOT(formatStrikeout()));
	connect(scFormatUnderline,	SIGNAL(activated()), this, SLOT(formatUnderline()));
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
	if(Notes->empty()) NewNotePlain();
	//
	connect(&SaveTimer, SIGNAL(timeout()), Notes, SLOT(SaveAll()));
	SaveTimer.start(15000);
	if(settings.getFileScanner())
	{
		connect(&ScanTimer, SIGNAL(timeout()), this, SLOT(scanForNewFiles()));
		ScanTimer.start(settings.getFileScannerTimeout());
	}
	connect(&settings, SIGNAL(FileScannerEnChanged(bool)), this, SLOT(fileScannerEnChanged(bool)));
	connect(&settings, SIGNAL(FileScannerTimeoutChanged(int)), this, SLOT(fileScannerTimeoutChanged(int)));
	//
	connect(&settings, SIGNAL(NotesPathChanged()), this, SLOT(notesPathChanged()));
	connect(&settings, SIGNAL(ShowHiddenChanged()), this, SLOT(warningSettingsChanged()));
	connect(&settings, SIGNAL(WindowStateChanged()), this, SLOT(windowStateChanged()));
	connect(&settings, SIGNAL(ToolbarItemsChanged()), this, SLOT(actions_changed()));
	connect(&settings.getScriptModel(), SIGNAL(dataChanged(const QModelIndex &, const QModelIndex &)),
		this, SLOT(cmd_changed()));
	if(!settings.getHideStart()) show();
}

MainWindow::~MainWindow()
{
	//saving notes
	Notes->SaveAll();
	//saving title of last note
	settings.setLastNote(Notes->current()->fileName());
	//saving dialog's params
	settings.setDialogGeometry(saveGeometry());
	settings.setDialogState(saveState());
	//saving scrits
	settings.setScripts();
	//syncing settings
	settings.save();
}

void MainWindow::currentNoteChanged(int old_index, int new_index)
{
	if(old_index!=-1)
	{
		Note* note = Notes->get(old_index);
		if(note->type()==Note::type_html) disconnect(note, SIGNAL(formatChanged(QFont)), 0, 0); //disconnecting old note
	}
	//
	actions[itemPrev]->setDisabled(new_index==0); //if first note
	actions[itemNext]->setDisabled(new_index==Notes->last()); //if last note
	actions[itemBack]->setEnabled(Notes->historyHasBack());
	actions[itemForward]->setEnabled(Notes->historyHasForward());
	//
	Note* note = Notes->current();
	switch(note->type())
	{
 case Note::type_html:
		{
			HtmlNote* htmlnote = dynamic_cast<HtmlNote*> (note);
			QFont font(htmlnote->getSelFormat().font());
			actions[itemFormatBold]->setChecked(font.bold());
			actions[itemFormatItalic]->setChecked(font.italic());
			actions[itemFormatStrikeout]->setChecked(font.strikeOut());
			actions[itemFormatUnderline]->setChecked(font.underline());
			connect(htmlnote, SIGNAL(formatChanged(QFont)), this, SLOT(formatChanged(QFont))); //connecting old note
			actions[itemFormatBold]->setEnabled(true);
			actions[itemFormatItalic]->setEnabled(true);
			actions[itemFormatStrikeout]->setEnabled(true);
			actions[itemFormatUnderline]->setEnabled(true);
			actions[itemFormatColor]->setEnabled(true);
		}
		break;
 default:
		{
			actions[itemFormatBold]->setEnabled(false);
			actions[itemFormatItalic]->setEnabled(false);
			actions[itemFormatStrikeout]->setEnabled(false);
			actions[itemFormatUnderline]->setEnabled(false);
			actions[itemFormatColor]->setEnabled(false);
		}
		break;
	}
}

void MainWindow::showSearchBar(bool show)
{
	ui->wSearch->setVisible(show);
	if(show)
	{
		ui->edSearch->setFocus();
		if(!actions[itemSearch]->isChecked()) actions[itemSearch]->setChecked(true);
	}
	else ui->edSearch->clear();
}

/*
  Searching text in notes
*/
void MainWindow::Search(bool next)
{
	Q_UNUSED(next);
	if(Notes->empty()) return;
	QString text(ui->edSearch->text());
	if(text.isEmpty()) return;
	Notes->search(text);
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

//Retranslating ui on language change
void MainWindow::changeEvent(QEvent *e)
{
	QMainWindow::changeEvent(e);
	switch (e->type())
	{
	case QEvent::LanguageChange:
		ui->retranslateUi(this);
		//
		actions[itemAdd]->setText(ToolbarAction(itemAdd).text());
		actions[itemRemove]->setText(ToolbarAction(itemRemove).text());
		actions[itemRename]->setText(ToolbarAction(itemRename).text());
		actions[itemPrev]->setText(ToolbarAction(itemPrev).text());
		actions[itemNext]->setText(ToolbarAction(itemNext).text());
		actions[itemCopy]->setText(ToolbarAction(itemCopy).text());
		actions[itemSetup]->setText(ToolbarAction(itemSetup).text());
		actions[itemInfo]->setText(ToolbarAction(itemInfo).text());
		actions[itemRun]->setText(ToolbarAction(itemRun).text());
		actions[itemSearch]->setText(ToolbarAction(itemSearch).text());
		actions[itemExit]->setText(ToolbarAction(itemExit).text());
		actShow->setText(tr("Show"));
		actHide->setText(tr("Hide"));
		//
		break;
	default: break;
	}
}

void MainWindow::on_btSearchClose_clicked()
{
	actions[itemSearch]->setChecked(false);
}
