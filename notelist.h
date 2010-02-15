#ifndef NOTELIST_H
#define NOTELIST_H

#include "note.h"

#include <QVector>
#include <QObject>
#include <QTabWidget>
#include <QSet>
#include <QMap>

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
	inline Note* current() const { return (current_index!=-1)?vec[current_index]:0; }
	inline Note* get(int i) const { return (vec.size()>i)?vec[i]:0; }
	inline void setCurrent(int n) { tabs->setCurrentIndex(n); }
	inline int currentIndex() const { return current_index; }
	//
	Note* add(const QFileInfo& fileinfo, bool set_current = true);
	bool load(const QFileInfo& fileinfo, const QString& old_title);
	void remove(int i);
	void rename(int index, const QString& title);
	//
	inline bool has(const QString& filename) const { return notes_filenames.contains(filename); }
	//
	void move(const QString& path);
	//
	void search(const QString& text);
	//
	inline QTabWidget* getWidget() const { return tabs; }
private:
	Note::Type getType(const QFileInfo& fileinfo) const;
public slots:
	void CurrentTabChanged(int index);
	void ShowExtensionsChanged(bool show_extensions);
	void TabPositionChanged();
	void SaveAll();
signals:
	void currentNoteChanged(int old_index, int new_index);
private:
	QSet<QString> notes_filenames;
	QMap<QString, Note::Type> NOTE_TYPE_MAP;
	QVector<Note*> vec;
	int current_index;
	QTabWidget* tabs;
};

#endif // NOTELIST_H
