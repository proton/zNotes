#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "settings.h"
#include "configdialog.h"
#include "aboutDialog.h"
#include "note_html.h"
#include "notecreatewidget.h"
#include "shared.h"

#include <QMessageBox>
#include <QClipboard>
#include <QProcess>
#include <QCloseEvent>
#include <QTextCharFormat>
#include <QColorDialog>

/*
	Tray icon on windows is very small
*/
#ifdef Q_WS_WIN
	#define TRAY_ICON_FILE_NAME ":/res/znotes.png"
#elif Q_WS_MAC
	#define TRAY_ICON_FILE_NAME ":/res/znotes32_bw.png"
#else
	#define TRAY_ICON_FILE_NAME ":/res/znotes32.png"
#endif

Settings settings;

// Small little wrapper receiver for the note_create_widget signal
void MainWindow::SetCheckedInverse(bool checked)
{
	actions[itemAdd]->setChecked(!checked);
}

void MainWindow::NewNote()
{
	if(!note_create_widget)
	{
		note_create_widget = new NoteCreateWidget(this, notes);
		connect(note_create_widget, SIGNAL(closed(bool)), this, SLOT(SetCheckedInverse(bool)));
		note_create_widget->setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint);
		note_create_widget->setFrameStyle(QFrame::StyledPanel | QFrame::Raised);
		note_create_widget->setFrameShadow(QFrame::Raised);
	}
	// TODO: Check if the toolbar is on the right side or bottom and 'flip' the note_create_widget
	if(ui->mainToolBar->orientation() == Qt::Horizontal)
		note_create_widget->move(ui->mainToolBar->mapToGlobal(QPoint(0,ui->mainToolBar->height())));
	else
		note_create_widget->move(ui->mainToolBar->mapToGlobal(QPoint(ui->mainToolBar->width(),0)));

	if(actions[itemAdd]->isChecked())
		note_create_widget->show();
	else
		note_create_widget->hide();
}

void MainWindow::NewNotePlain()
{
	notes->create(Note::type_text);
}

void MainWindow::NewNoteHTML()
{
	notes->create(Note::type_html);
}

void MainWindow::NewNoteTODO()
{
	notes->create(Note::type_todo);
}

void MainWindow::PreviousNote()
{
	if(notes->empty()) return;
	notes->getWidget()->setCurrentIndex(notes->currentIndex()-1);
}

void MainWindow::NextNote()
{
	if(notes->empty()) return;
	notes->getWidget()->setCurrentIndex(notes->currentIndex()+1);
}

void MainWindow::ToNote(int n)
{
	if(n>=notes->count()) return;
	notes->getWidget()->setCurrentIndex(n);
}

void MainWindow::CopyNote()
{
	if(notes->empty()) return;
	notes->current()->copy();
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

void MainWindow::hideEvent(QHideEvent* event)
{
	Q_UNUSED(event)
	settings.setDialogGeometry(saveGeometry());
	actShow->setEnabled(true);
	actHide->setDisabled(true);
	if(notes->current()) notes->saveAll();

	if(note_create_widget && note_create_widget->isVisible())
	{
		actions[itemAdd]->setChecked(false);
		note_create_widget->hide();
	}
}

void MainWindow::showEvent(QShowEvent* event)
{
	Q_UNUSED(event)
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

	if(note_create_widget && note_create_widget->isVisible())
	{
		actions[itemAdd]->setChecked(false);
		note_create_widget->hide();
	}
}

void MainWindow::moveEvent(QMoveEvent* event)
{
	Q_UNUSED(event)
	if(note_create_widget && note_create_widget->isVisible())
	{
		// TODO: Check if the toolbar is on the right side or bottom and 'flip' the note_create_widget
		if(ui->mainToolBar->orientation() == Qt::Horizontal)
			note_create_widget->move(ui->mainToolBar->mapToGlobal(QPoint(0,ui->mainToolBar->height())));
		else
			note_create_widget->move(ui->mainToolBar->mapToGlobal(QPoint(ui->mainToolBar->width(),0)));
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

void MainWindow::windowStateChanged()
{
	Qt::WindowFlags flags = Qt::Window;
	if(settings.getHideFrame()) flags |= Qt::FramelessWindowHint;
	if(settings.getStayTop()) flags |= Qt::WindowStaysOnTopHint;
	bool window_is_visible = isVisible();
	setWindowFlags(flags);
	if(window_is_visible) show();
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
	if(notes->empty()) return;
	QProcess p;
	QStringList args;
	args << notes->current()->absolutePath();
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
	cmd_menu.addSeparator();
	cmd_menu.addAction(tr("Edit command list"), this, SLOT(edit_command_list()));
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

void MainWindow::edit_command_list()
{
	configDialog dlg;
	dlg.changeTabToCommands();
	dlg.exec();
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
	if(notes->empty()) return;
	if(notes->current()->type()!=Note::type_html) return;
	QTextCharFormat format;
	bool is_bold = actions[itemFormatBold]->isChecked();
	format.setFontWeight(is_bold?QFont::Bold : QFont::Normal);
	notes->current()->setSelFormat(format);
}

void MainWindow::formatItalic()
{
	if(notes->empty()) return;
	if(notes->current()->type()!=Note::type_html) return;
	QTextCharFormat format;
	bool is_italic = actions[itemFormatItalic]->isChecked();
	format.setFontItalic(is_italic);
	notes->current()->setSelFormat(format);
}

void MainWindow::formatStrikeout()
{
	if(notes->empty()) return;
	if(notes->current()->type()!=Note::type_html) return;
	QTextCharFormat format;
	bool is_strikeout = actions[itemFormatStrikeout]->isChecked();
	format.setFontStrikeOut(is_strikeout);
	notes->current()->setSelFormat(format);
}

void MainWindow::formatUnderline()
{
	if(notes->empty()) return;
	if(notes->current()->type()!=Note::type_html) return;
	QTextCharFormat format;
	bool is_underline = actions[itemFormatUnderline]->isChecked();
	format.setFontUnderline(is_underline);
	notes->current()->setSelFormat(format);
}

void MainWindow::formatTextColor()
{
	if(notes->empty()) return;
	Note* note = notes->current();
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

#include <QtDebug>

MainWindow::MainWindow(QWidget *parent)
	: QMainWindow(parent), ui(new Ui::MainWindow), note_create_widget(0)
{
	ui->setupUi(this);
	ui->wSearch->hide();
	//restoring window state
	restoreGeometry(settings.getDialogGeometry());
	restoreState(settings.getDialogState());
	windowStateChanged();

	notes = new NoteList(ui->centralWidget);
	ui->layout->addWidget(notes->getWidget());
	connect(notes, SIGNAL(currentNoteChanged(int,int)), this, SLOT(currentNoteChanged(int,int)));

	//Creating toolbar/menu actions
	for(int i=0; i<itemMax ; ++i) actions[i] = GenerateAction(i);
	actShow =	new QAction(tr("Show"),	parent);
	actHide =	new QAction(tr("Hide"),	parent);
	//Connecting actions with slots
	actions[itemAdd]->setCheckable(true);
	connect(actions[itemAdd],		SIGNAL(triggered()), this, SLOT(NewNote()));
	connect(actions[itemAddText],		SIGNAL(triggered()), this, SLOT(NewNotePlain()));
	connect(actions[itemAddHtml],	SIGNAL(triggered()), this, SLOT(NewNoteHTML()));
	connect(actions[itemAddTodo],	SIGNAL(triggered()), this, SLOT(NewNoteTODO()));
	connect(actions[itemRemove],	SIGNAL(triggered()), notes, SLOT(removeCurrentNote()));
	connect(actions[itemRename],	SIGNAL(triggered()), notes, SLOT(renameCurrentNote()));
	connect(actions[itemPrev],	SIGNAL(triggered()), this, SLOT(PreviousNote()));
	connect(actions[itemNext],	SIGNAL(triggered()), this, SLOT(NextNote()));
	connect(actions[itemBack],	SIGNAL(triggered()), notes, SLOT(historyBack()));
	connect(actions[itemForward],	SIGNAL(triggered()), notes, SLOT(historyForward()));
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
	Shared::addShortcut(new QShortcut(QKeySequence::New, this), tr("Create new note"));
		connect(Shared::shortcuts().last(), SIGNAL(activated()), this, SLOT(NewNote()));
	Shared::addShortcut(new QShortcut(QKeySequence::Close, this));
		connect(Shared::shortcuts().last(), SIGNAL(activated()), notes, SLOT(removeCurrentNote()));
	Shared::addShortcut(new QShortcut(Qt::Key_F2, this), tr("Rename current note"));
		connect(Shared::shortcuts().last(), SIGNAL(activated()), notes, SLOT(renameCurrentNote()));
	Shared::addShortcut(new QShortcut(QKeySequence::Back, this), tr("Go to previous note"));
		connect(Shared::shortcuts().last(), SIGNAL(activated()), this, SLOT(PreviousNote()));
	Shared::addShortcut(new QShortcut(QKeySequence::Forward, this), tr("Go to next note"));
		connect(Shared::shortcuts().last(), SIGNAL(activated()), this, SLOT(NextNote()));
	Shared::addShortcut(new QShortcut(QKeySequence::PreviousChild, this));
		connect(Shared::shortcuts().last(), SIGNAL(activated()), notes, SLOT(historyBack()));
	Shared::addShortcut(new QShortcut(QKeySequence::NextChild, this));
		connect(Shared::shortcuts().last(), SIGNAL(activated()), notes, SLOT(historyForward()));
	Shared::addShortcut(new QShortcut(QKeySequence::Find, this), tr("Search in the notes' text"));
		connect(Shared::shortcuts().last(), SIGNAL(activated()), actions[itemSearch], SLOT(toggle()));
	Shared::addShortcut(new QShortcut(Qt::Key_Escape, ui->edSearch, 0, 0, Qt::WidgetShortcut));
		connect(Shared::shortcuts().last(), SIGNAL(activated()), actions[itemSearch], SLOT(toggle()));
	Shared::addShortcut(new QShortcut(Qt::CTRL + Qt::Key_Q, this), tr("Exit program"));
		connect(Shared::shortcuts().last(), SIGNAL(activated()), qApp, SLOT(quit()));
	Shared::addShortcut(new QShortcut(QKeySequence::Bold, this), tr("Make selected text bold"));
		connect(Shared::shortcuts().last(), SIGNAL(activated()), this, SLOT(formatBold()));
	Shared::addShortcut(new QShortcut(QKeySequence::Italic, this), tr("Make selected text italic"));
		connect(Shared::shortcuts().last(), SIGNAL(activated()), this, SLOT(formatItalic()));
	Shared::addShortcut(new QShortcut(Qt::CTRL + Qt::Key_S, this), tr("Make selected text strikeout"));
		connect(Shared::shortcuts().last(), SIGNAL(activated()), this, SLOT(formatStrikeout()));
	Shared::addShortcut(new QShortcut(QKeySequence::Underline, this), tr("Make selected text underline"));
		connect(Shared::shortcuts().last(), SIGNAL(activated()), this, SLOT(formatUnderline()));

	for(int i=1; i<=9; ++i) //from Alt+1 to Alt+9
	{
		QShortcut* shortcut = new QShortcut(QKeySequence(Qt::ALT + Qt::Key_0+i), this);
		connect(shortcut, SIGNAL(activated()), &alt_mapper, SLOT(map()));
		alt_mapper.setMapping(shortcut, i-1);
	}
	connect(&alt_mapper, SIGNAL(mapped(int)), this, SLOT(ToNote(int)));
	//
	connect(&settings, SIGNAL(ShowHiddenChanged()), this, SLOT(warningSettingsChanged()));
	connect(&settings, SIGNAL(WindowStateChanged()), this, SLOT(windowStateChanged()));
	connect(&settings, SIGNAL(ToolbarItemsChanged()), this, SLOT(actions_changed()));
	connect(&settings.getScriptModel(), SIGNAL(dataChanged(const QModelIndex&, const QModelIndex&)),
		this, SLOT(cmd_changed()));
	if(!settings.getHideStart()) show();
	//
	currentNoteChanged(-1, notes->currentIndex());
}

MainWindow::~MainWindow()
{
	//saving notes
	notes->saveAll();
	//saving title of last note
	if(notes->current()) settings.setLastNote(notes->current()->fileName());
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
		Note* note = notes->get(old_index);
		if(note->type()==Note::type_html) disconnect(note, SIGNAL(formatChanged(QFont)), 0, 0); //disconnecting old note
	}
	//
	bool not_empty = !notes->empty();
	actions[itemPrev]->setEnabled(not_empty && new_index!=0); //if first note
	actions[itemNext]->setEnabled(not_empty && new_index!=notes->last()); //if last note
	actions[itemBack]->setEnabled(not_empty && notes->historyHasBack());
	actions[itemForward]->setEnabled(not_empty && notes->historyHasForward());
	actions[itemRemove]->setEnabled(not_empty);
	//
	Note* note = notes->current();
	if(note)
	{
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
	else
	{
		actions[itemFormatBold]->setEnabled(false);
		actions[itemFormatItalic]->setEnabled(false);
		actions[itemFormatStrikeout]->setEnabled(false);
		actions[itemFormatUnderline]->setEnabled(false);
		actions[itemFormatColor]->setEnabled(false);
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
	if(notes->empty()) return;
	QString text(ui->edSearch->text());
	if(text.isEmpty()) return;
	notes->search(text);
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
		notes->retranslate(settings.getLocaleCurrent());
		break;
	default: break;
	}
}

void MainWindow::on_btSearchClose_clicked()
{
	actions[itemSearch]->setChecked(false);
}
