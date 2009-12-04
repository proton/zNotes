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

#include "note.h"
#include "settings.h"

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
	QVector<Note*> Notes;
	int CurrentIndex;
	QSystemTrayIcon tray;
	QMenu cmenu;
	QMenu cmd_menu;
	QSignalMapper cmd_mapper;
	QSignalMapper alt_mapper;
	QTimer SaveTimer;
	//
	QAction *actAdd, *actRemove, *actRename, *actPrev, *actNext;
	QAction *actCopy, *actSetup, *actRun, *actExit, *actInfo, *actSearch;
	//
	QShortcut *scAdd, *scRemove, *scPrev, *scNext, *scExit, *scSearch;
	//
	void LoadNotes();
	inline Note* currentNote()
	{
		return Notes[CurrentIndex];
	}
	int currentIndex;
	void SaveNote(int i);
	//
	void Search(bool next);
	inline QAction* getAction(int i)
	{
		switch(i)
		{
			case itemAdd: return actAdd;
			case itemRemove: return actRemove;
			case itemRename: return actRename;
			case itemPrev: return actPrev;
			case itemNext: return actNext;
			case itemCopy: return actCopy;
			case itemSetup: return actSetup;
			case itemInfo: return actInfo;
			case itemRun: return actRun;
			case itemSearch: return actSearch;
			case itemExit: return actExit;
			default: return new QAction(this);
		}
	}
public slots:
	void SaveCurrentNote();
	void RemoveCurrentNote();
	void RenameCurrentNote();
	void NewNote();
	//
	void PreviousNote();
	void NextNote();
	void ToNote(int n);
	//
	void CopyNote();
	//
	void currentNoteChanged();
	void on_tabs_currentChanged(int index);
	//
	void SaveAll();
	//
	void trayActivated(QSystemTrayIcon::ActivationReason reason);
	//
	void commandMenu();
	void showAboutDialog();
	void showPrefDialog();
	void showSearchBar();
	//
	void actions_changed();
	//
	void cmd_changed();
	void cmdExec(const QString &);
	//
	void notesPathChanged();
	void windowStateChanged();
	void toolbarVisChanged();//TODO:think
	void noteFontChanged();
private slots:
	void on_edSearch_returnPressed();
	void on_edSearch_textChanged(QString text);
};

#endif // MAINWINDOW_H
