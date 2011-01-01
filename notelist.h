#ifndef NOTELIST_H
#define NOTELIST_H

#include "note.h"

#include <QVector>
#include <QList>
#include <QObject>
#include <QTabWidget>
#include <QSet>
#include <QMap>
#include <QDir>

class QFileSystemWatcher;

class NoteList : public QObject
{
	Q_OBJECT
public:
	NoteList(QWidget* parent = 0);
	~NoteList();
	//
	inline bool empty() const {return vec.isEmpty(); }
	inline int count() const { return vec.size(); }
	inline int last() const { return vec.size()-1; }
	inline void setCurrent(int n) { tabs->setCurrentIndex(n); }
	inline Note* current() const { return (current_index!=-1)?vec[current_index]:0; }
	inline Note* get(int i) const { return (vec.size()>i)?vec[i]:0; }
	inline int currentIndex() const { return current_index; }
	inline bool historyHasBack() const { return history_index>0; }
	inline bool historyHasForward() const { return (history_index+1)<history.size(); }
	//
	void create(const QString& mask);
	//
	void search(const QString& text);
	//
	inline QTabWidget* getWidget() const { return tabs; }
	//
	void retranslate(const QLocale& locale);
private:
	void initNoteTypes();
	Note::Type getType(const QFileInfo& fileinfo) const;
	Note* add(const QFileInfo& fileinfo, bool set_current = true);
	void remove(int i);
	void rename(int index, const QString& title);
	void move(const QString& path);
public slots:
	void saveAll();
	//
	void removeCurrentNote();
	void renameCurrentNote();
	//
	void historyBack();
	void historyForward();
private slots:
	void currentTabChanged(int index);
	void showExtensionsChanged(bool show_extensions);
	void tabPositionChanged();
	void notesPathChanged();
	void scanForNewFiles();
signals:
	void currentNoteChanged(int old_index, int new_index);
private:
	QSet<QString> notes_filenames;
	QMap<QString, Note::Type> NOTE_TYPE_MAP;
	QVector<Note*> vec;
	//
	QList<QString> history;
	int history_index;
	bool history_forward_pressed;
	bool history_back_pressed;
	//
	int current_index;
	QTabWidget* tabs;
	//
	QFileSystemWatcher* watcher;
	QDir dir;
};

#endif // NOTELIST_H
