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
			actRemove->setDisabled(true);
			actRename->setDisabled(true);
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
		Notes->rename(Notes->currentIndex(), new_name);
	}
}

void MainWindow::NewNote()
{
	int n = 0;
	QString filename = QString::number(n);
	QFile file(dir.absoluteFilePath(filename));
	while(file.exists()) //Searching for free filename
	{
		filename = QString::number(++n);
		file.setFileName(dir.absoluteFilePath(filename));
	}
	Notes->add(file);
	if(!Notes->empty())
	{
		actRemove->setEnabled(true);
		actRename->setEnabled(true);
	}
}

void MainWindow::NewNoteHTML()
{
	int n = 0;
	QString filename = QString("%1.htm").arg(n);
	QFile file(dir.absoluteFilePath(filename));
	while(file.exists()) //Searching for free filename
	{
		filename = QString("%1.htm").arg(++n);
		file.setFileName(dir.absoluteFilePath(filename));
	}
	Notes->add(file);
	if(!Notes->empty())
	{
		actRemove->setEnabled(true);
		actRename->setEnabled(true);
	}
}

void MainWindow::NewNoteTODO()
{
	int n = 0;
	QString filename = QString("%1.ztodo").arg(n);
	QFile file(dir.absoluteFilePath(filename));
	while(file.exists()) //Searching for free filename
	{
		filename = QString("%1.ztodo").arg(++n);
		file.setFileName(dir.absoluteFilePath(filename));
	}
	Notes->add(file);
	if(!Notes->empty())
	{
		actRemove->setEnabled(true);
		actRename->setEnabled(true);
	}
}

void MainWindow::PreviousNote()
{
	Notes->getWidget()->setCurrentIndex(Notes->currentIndex()-1);
}

void MainWindow::NextNote()
{
	Notes->getWidget()->setCurrentIndex(Notes->currentIndex()+1);
}

void MainWindow::ToNote(int n)
{
	if(n>=Notes->count()) return;
	Notes->getWidget()->setCurrentIndex(n);
}

void MainWindow::CopyNote()
{
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
	actShow->setEnabled(true);
	actHide->setDisabled(true);
	settings.setDialogGeometry(saveGeometry());
	if(Notes->current()!=0) Notes->SaveAll();
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
		else ui->mainToolBar->addAction(getAction(items[i]));
	}
}

void MainWindow::formatChanged(const QFont& font)
{
	actFormatBold->setChecked(font.bold());
	actFormatItalic->setChecked(font.italic());
	actFormatStrikeout->setChecked(font.strikeOut());
	actFormatUnderline->setChecked(font.underline());
}

void MainWindow::formatBold()
{
	if(Notes->empty()) return;
	if(Notes->current()->type()!=Note::type_html) return;
	QTextCharFormat format;
	bool is_bold = actFormatBold->isChecked();
	format.setFontWeight(is_bold?QFont::Bold : QFont::Normal);
	Notes->current()->setSelFormat(format);
}

void MainWindow::formatItalic()
{
	if(Notes->empty()) return;
	if(Notes->current()->type()!=Note::type_html) return;
	QTextCharFormat format;
	bool is_italic = actFormatItalic->isChecked();
	format.setFontItalic(is_italic);
	Notes->current()->setSelFormat(format);
}

void MainWindow::formatStrikeout()
{
	if(Notes->empty()) return;
	if(Notes->current()->type()!=Note::type_html) return;
	QTextCharFormat format;
	bool is_strikeout = actFormatStrikeout->isChecked();
	format.setFontStrikeOut(is_strikeout);
	Notes->current()->setSelFormat(format);
}

void MainWindow::formatUnderline()
{
	if(Notes->empty()) return;
	if(Notes->current()->type()!=Note::type_html) return;
	QTextCharFormat format;
	bool is_underline = actFormatUnderline->isChecked();
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
inline QAction* GenerateAction(item_enum item, bool checkable = false)
{
	ToolbarAction toolbar_action(item);
	QAction* action = new QAction(toolbar_action.icon(), toolbar_action.text(), 0);
	action->setCheckable(checkable);
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
	actAdd = GenerateAction(itemAdd);
	actAddHtml = GenerateAction(itemAddHtml);
	actAddTodo = GenerateAction(itemAddTodo);
	actRemove = GenerateAction(itemRemove);
	actRename = GenerateAction(itemRename);
	actPrev = GenerateAction(itemPrev);
	actNext = GenerateAction(itemNext);
	actCopy = GenerateAction(itemCopy);
	actSetup = GenerateAction(itemSetup);
	actInfo = GenerateAction(itemInfo);
	actRun = GenerateAction(itemRun);
	actSearch = GenerateAction(itemSearch, true);
	actExit = GenerateAction(itemExit);
	actFormatBold = GenerateAction(itemFormatBold, true);
	actFormatItalic = GenerateAction(itemFormatItalic, true);
	actFormatStrikeout = GenerateAction(itemFormatStrikeout, true);
	actFormatUnderline = GenerateAction(itemFormatUnderline, true);
	actFormatColor = GenerateAction(itemFormatColor);
	actShow =	new QAction(tr("Show"),	parent);
	actHide =	new QAction(tr("Hide"),	parent);
	//Connecting actions with slots
	connect(actAdd,		SIGNAL(triggered()), this, SLOT(NewNote()));
	connect(actAddHtml,	SIGNAL(triggered()), this, SLOT(NewNoteHTML()));
	connect(actAddHtml,	SIGNAL(triggered()), this, SLOT(NewNoteTODO()));
	connect(actRemove,	SIGNAL(triggered()), this, SLOT(RemoveCurrentNote()));
	connect(actRename,	SIGNAL(triggered()), this, SLOT(RenameCurrentNote()));
	connect(actPrev,	SIGNAL(triggered()), this, SLOT(PreviousNote()));
	connect(actNext,	SIGNAL(triggered()), this, SLOT(NextNote()));
	connect(actCopy,	SIGNAL(triggered()), this, SLOT(CopyNote()));
	connect(actSetup,	SIGNAL(triggered()), this, SLOT(showPrefDialog()));
	connect(actInfo,	SIGNAL(triggered()), this, SLOT(showAboutDialog()));
	connect(actRun,		SIGNAL(triggered()), this, SLOT(commandMenu()));
	connect(actSearch,	SIGNAL(triggered(bool)), this, SLOT(showSearchBar(bool)));
	connect(actExit,	SIGNAL(triggered()), qApp, SLOT(quit()));
	connect(actFormatBold,	SIGNAL(triggered()), this, SLOT(formatBold()));
	connect(actFormatItalic,	SIGNAL(triggered()), this, SLOT(formatItalic()));
	connect(actFormatStrikeout,	SIGNAL(triggered()), this, SLOT(formatStrikeout()));
	connect(actFormatUnderline,	SIGNAL(triggered()), this, SLOT(formatUnderline()));
	connect(actFormatColor,	SIGNAL(triggered()), this, SLOT(formatTextColor()));
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
	scRename =	new QShortcut(Qt::Key_F2,	this);
	scBack =	new QShortcut(QKeySequence::Back,	this);
	scForward =	new QShortcut(QKeySequence::Forward,this);
	scPrev =	new QShortcut(QKeySequence::PreviousChild,	this);
	scNext =	new QShortcut(QKeySequence::NextChild,this);
	scSearch =	new QShortcut(QKeySequence::Find,	this);
	scExit =	new QShortcut(QKeySequence::Close,	this);
	scFormatBold =		new QShortcut(Qt::CTRL + Qt::Key_B,	this);
	scFormatItalic =	new QShortcut(Qt::CTRL + Qt::Key_I,	this);
	scFormatStrikeout = new QShortcut(Qt::CTRL + Qt::Key_S,	this);
	scFormatUnderline = new QShortcut(Qt::CTRL + Qt::Key_U,	this);
	//Connecting shortcuts with slots
	connect(scAdd,		SIGNAL(activated()), this, SLOT(NewNote()));
	connect(scRemove,	SIGNAL(activated()), this, SLOT(RemoveCurrentNote()));
	connect(scRename,	SIGNAL(activated()), this, SLOT(RenameCurrentNote()));
	connect(scBack,		SIGNAL(activated()), this, SLOT(PreviousNote()));
	connect(scForward,	SIGNAL(activated()), this, SLOT(NextNote()));
	connect(scPrev,		SIGNAL(activated()), this, SLOT(PreviousNote()));
	connect(scNext,		SIGNAL(activated()), this, SLOT(NextNote()));
	connect(scSearch,	SIGNAL(activated()), this, SLOT(showSearchBar()));
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
	if(Notes->empty()) NewNote();
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
	delete ui;
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
	//
	delete Notes;
}

void MainWindow::currentNoteChanged(int old_index, int new_index)
{
	if(old_index!=-1)
	{
		Note* note = Notes->get(old_index);
		if(note->type()==Note::type_html) disconnect(note, SIGNAL(formatChanged(QFont)), 0, 0); //disconnecting old note
	}
	//
	actPrev->setDisabled(new_index==0); //if first note
	actNext->setDisabled(new_index==Notes->last()); //if last note
	//
	Note* note = Notes->current();
	switch(note->type())
	{
 case Note::type_html:
		{
			HtmlNote* htmlnote = dynamic_cast<HtmlNote*> (note);
			QFont font(htmlnote->getSelFormat().font());
			actFormatBold->setChecked(font.bold());
			actFormatItalic->setChecked(font.italic());
			actFormatStrikeout->setChecked(font.strikeOut());
			actFormatUnderline->setChecked(font.underline());
			connect(htmlnote, SIGNAL(formatChanged(QFont)), this, SLOT(formatChanged(QFont))); //connecting old note
			actFormatBold->setEnabled(true);
			actFormatItalic->setEnabled(true);
			actFormatStrikeout->setEnabled(true);
			actFormatUnderline->setEnabled(true);
			actFormatColor->setEnabled(true);
		}
		break;
 default:
		{
			actFormatBold->setEnabled(false);
			actFormatItalic->setEnabled(false);
			actFormatStrikeout->setEnabled(false);
			actFormatUnderline->setEnabled(false);
			actFormatColor->setEnabled(false);
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
		if(!actSearch->isChecked()) actSearch->setChecked(true);
	}
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
