#include "notelist.h"

NoteList::NoteList()
	: vec(), current_index(-1)
{
}

void NoteList::remove(int i)
{
	vec.remove(i);
	//
}
