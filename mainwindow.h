#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui/QMainWindow>
#include <QSystemTrayIcon>
#include <QMenu>
//#include <QTimer>
#include <QSignalMapper>
#include <QAction>
#include <QShortcut>
#include <QSessionManager>

#include "notelist.h"
#include "settings.h"
#include "toolbaraction.h"

class NoteCreateWidget;

namespace Ui
{
	class MainWindow;
}

class MainWindow : public QMainWindow
{
	Q_OBJECT
public:
	MainWindow(QWidget *parent = 0);
	~MainWindow();
protected:
	void closeEvent(QCloseEvent *event);
	void hideEvent(QHideEvent *event);
	void showEvent(QShowEvent *event);
private:
	Ui::MainWindow *ui;
	//
	NoteList* notes;
	NoteCreateWidget* note_create_widget;
	//
	QSystemTrayIcon tray;
	QMenu cmenu;
	QMenu cmd_menu;
	QSignalMapper cmd_mapper;
	QSignalMapper alt_mapper;
	//QTimer SaveTimer;
	//
	QAction* actions[itemMax];
//	//
	QAction *actShow, *actHide;
//	//
	QShortcut *scAdd, *scRemove, *scRename, *scBack, *scForward, *scPrev, *scNext, *scExit, *scSearch;
	QShortcut *scFormatBold, *scFormatItalic, *scFormatStrikeout, *scFormatUnderline;
	QShortcut *scSearchEsc;
	//
	void LoadNotes();
	//
	void Search(bool next);
	void changeEvent(QEvent *e);
public slots:
	void NewNote();
	void NewNotePlain();
	void NewNoteHTML();
	void NewNoteTODO();
	//
	void PreviousNote();
	void NextNote();
	void ToNote(int n);
	//
	void CopyNote();
	//
	void currentNoteChanged(int old_index, int new_index);
	//
	void trayActivated(QSystemTrayIcon::ActivationReason reason);
	//
	void commandMenu();
	void showAboutDialog();
	void showPrefDialog();
	void showSearchBar(bool show = true);
	//
	void formatChanged(const QFont& font);
	void formatBold();
	void formatItalic();
	void formatStrikeout();
	void formatUnderline();
	void formatTextColor();
	//
	void actions_changed();
	//
	void cmd_changed();
	void cmdExec(const QString &);
	//
	void windowStateChanged();
	//
//	void scanForNewFiles();
	//
	void warningSettingsChanged();
private slots:
	void on_btSearchClose_clicked();
	void on_edSearch_returnPressed();
	void on_edSearch_textChanged(QString text);
};

#endif // MAINWINDOW_H
