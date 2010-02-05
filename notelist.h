#ifndef NOTELIST_H
#define NOTELIST_H

#include "note.h"

#include <QVector>
#include <QObject>
#include <QTabWidget>

class NoteList : public QObject
{
	Q_OBJECT
public:
	NoteList(QWidget* parent = 0);
	//
	inline bool empty() const {return vec.isEmpty(); }
	inline int count() const { return vec.size(); }
	inline int last() const { return vec.size()-1; }
	inline Note* current() const { return (current_index!=-1)?vec[current_index]:0; }
	inline Note* get(int i) const { return (vec.size()>i)?vec[i]:0; }
	inline void setCurrent(int n) { tabs->setCurrentIndex(n); }
	inline int currentIndex() const { return current_index; }
	//
	void add(const QFileInfo& fileinfo);
	bool load(const QFileInfo& fileinfo, const QString& old_title);
	void remove(int i);
	void rename(int index, const QString& title);
	//
	void move(const QString& path);
	//
	void search(const QString& text);
	//
	inline QTabWidget* getWidget() const { return tabs; }
public slots:
	void CurrentTabChanged(int index);
	void ShowExtensionsChanged(bool show_extensions);
	void SaveAll();
signals:
	void currentNoteChanged(int old_index, int new_index);
private:
	QVector<Note*> vec;
	int current_index;
	QTabWidget* tabs;
};

#endif // NOTELIST_H
