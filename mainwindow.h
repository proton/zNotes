#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui/QMainWindow>
#include <QDir>
#include <QFile>
#include <QPlainTextEdit>
#include <QSystemTrayIcon>
#include <QMenu>
#include <QTimer>

class Note : public QPlainTextEdit
{
	public:
		Note(const QString& fn, const QDir&);
		QString name;
		void load(const QString&);
		QFile file;
		bool hasChange;
};

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
	Note* currentNote();
	int currentIndex;
	void SaveNote(int i);
public slots:
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

private slots:
	void on_tabs_currentChanged(int index);
};

#endif // MAINWINDOW_H
