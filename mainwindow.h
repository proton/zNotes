#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui/QMainWindow>
#include <QDir>
#include <QSystemTrayIcon>
#include <QMenu>
#include <QTimer>

#include "note.h"

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
	QTimer SaveTimer;
	//
	void LoadNotes();
	inline Note* currentNote()
	{
		return Notes[CurrentIndex];
	}
	int currentIndex;
	void SaveNote(int i);
private slots:
	void SaveCurrentNote();
	void RemoveCurrentNote();
	void RenameCurrentNote();
	void NewNote();
	//
	void PreviousNote();
	void NextNote();
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
	void aboutDialog();
	void prefDialog();
	//
	void notesPathChanged();
	void windowStateChanged();
	void toolbarVisChanged();
	void noteFontChanged();
};

#endif // MAINWINDOW_H
