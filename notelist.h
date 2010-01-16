#ifndef NOTELIST_H
#define NOTELIST_H

#include "note.h"

#include <QVector>

class NoteList
{
public:
	NoteList();
	//
	inline bool empty() const {return vec.isEmpty(); }
	inline int count() const { return vec.size(); }
	inline Note* current() const { return (current_index!=-1)?vec[current_index]:0; }
	inline Note* operator[](int i) const { return (vec.size()>i)?vec[i]:0; }
	inline void add(Note* n) { vec.append(n); }
	inline void setCurrent(int n) { current_index=n; }
	inline int currentIndex() const { return current_index; }
	//
	void remove(int i);
private:
	QVector<Note*> vec;
	int current_index;
};

#endif // NOTELIST_H
