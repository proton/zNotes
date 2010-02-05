#include "notelist.h"
#include "settings.h"

NoteList::NoteList()
	: QObject(), vec(), current_index(-1)
{
	connect(&settings, SIGNAL(ShowExtensionsChanged(bool)), this, SLOT(ShowExtensionsChanged(bool)));
}

void NoteList::remove(int i)
{
	vec.remove(i);
	//
}

void NoteList::ShowExtensionsChanged(bool show_extensions)
{
	for(int i=0; i<vec.size(); ++i)
	{
		//TODO: update tabbar
		vec[i]->setTitle(show_extensions);
	}
}
