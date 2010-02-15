#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui/QMainWindow>
#include <QDir>
#include <QSystemTrayIcon>
#include <QMenu>
#include <QTimer>
#include <QSignalMapper>
#include <QAction>
#include <QShortcut>
#include <QSessionManager>

#include "notelist.h"
#include "settings.h"
#include "toolbaraction.h"

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
	QDir dir;
	NoteList* Notes;
	//
	QSystemTrayIcon tray;
	QMenu cmenu;
	QMenu cmd_menu;
	QSignalMapper cmd_mapper;
	QSignalMapper alt_mapper;
	QTimer SaveTimer;
	QTimer ScanTimer;
	//
	QAction *actAdd, *actAddHtml, *actRemove, *actRename, *actPrev, *actNext;
	QAction *actCopy, *actSetup, *actRun, *actExit, *actInfo, *actSearch;
	QAction *actFormatBold, *actFormatItalic, *actFormatStrikeout, *actFormatUnderline, *actFormatColor;
	//
	QAction *actShow, *actHide;
	//
	QShortcut *scAdd, *scRemove, *scRename, *scBack, *scForward, *scPrev, *scNext, *scExit, *scSearch;
	QShortcut *scFormatBold, *scFormatItalic, *scFormatStrikeout, *scFormatUnderline;
	//
	void LoadNotes();
	//
	void Search(bool next);
	inline QAction* getAction(int i)
	{
		switch(i)
		{
			case itemAdd:			return actAdd;
			case itemAddHtml:		return actAddHtml;
			case itemRemove:		return actRemove;
			case itemRename:		return actRename;
			case itemPrev:			return actPrev;
			case itemNext:			return actNext;
			case itemCopy:			return actCopy;
			case itemSetup:			return actSetup;
			case itemInfo:			return actInfo;
			case itemRun:			return actRun;
			case itemSearch:		return actSearch;
			case itemExit:			return actExit;
			case itemFormatBold:	return actFormatBold;
			case itemFormatItalic:	return actFormatItalic;
			case itemFormatStrikeout: return actFormatStrikeout;
			case itemFormatUnderline: return actFormatUnderline;
			case itemFormatColor:	return actFormatColor;
			default: return new QAction(this);
		}
	}
	void changeEvent(QEvent *e);
public slots:
	void RemoveCurrentNote();
	void RenameCurrentNote();
	void NewNote();
	void NewNoteHTML();
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
	void notesPathChanged();
	void fileScannerEnChanged(bool);
	void fileScannerTimeoutChanged(int);
	void windowStateChanged();
	//
	void scanForNewFiles();
	//
	void warningSettingsChanged();
private slots:
	void on_edSearch_returnPressed();
	void on_edSearch_textChanged(QString text);
};

#endif // MAINWINDOW_H
