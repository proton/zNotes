#include "notelist.h"
#include "settings.h"

NoteList::NoteList(QWidget* parent)
	: QObject(), vec(), current_index(-1)
{
	tabs = new QTabWidget(parent);
	connect(tabs, SIGNAL(currentChanged(int)), this, SLOT(CurrentTabChanged(int)));
	connect(&settings, SIGNAL(ShowExtensionsChanged(bool)), this, SLOT(ShowExtensionsChanged(bool)));
}

void NoteList::add(const QFileInfo& fileinfo)
{
	Note* note = new Note(fileinfo);
	vec.append(note);
	tabs->addTab(note->widget(), note->title());
	tabs->setCurrentWidget(note->widget());
}

bool NoteList::load(const QFileInfo& fileinfo, const QString& old_title)
{
	Note* note = new Note(fileinfo);
	vec.append(note);
	tabs->addTab(note->widget(), note->title());
	return (note->title()==old_title);
}

void NoteList::remove(int i)
{
	Note* current_note = current();
	tabs->removeTab(i);
	delete current_note;
	vec.remove(i);
}

void NoteList::move(const QString& path)
{
	for(int i=0; i<vec.size(); ++i)
	{
		vec[i]->move(path);
	}
}

void NoteList::search(const QString& text)
{
	//Searching in current note
	if(current()->find(text)) return;
	//Searching in all notes
	const int max = count()+currentIndex();
	for(int n=currentIndex()+1; n<=max; ++n)
	{
		int i = n%vec.size(); //secret formula of success search
		if(vec[i]->find(text, true))
		{
			tabs->setCurrentIndex(i);
			return;
		}
	}
}

void NoteList::rename(int index, const QString& title)
{
	Note* note = vec[index];
	note->save();
	note->rename(title);
	tabs->setTabText(index, note->title());
}

void NoteList::CurrentTabChanged(int index)
{
	if(current_index!=-1) current()->save();
	int old_index = current_index;
	current_index = index;
	emit currentNoteChanged(old_index, current_index);
}

void NoteList::ShowExtensionsChanged(bool show_extensions)
{
	for(int i=0; i<vec.size(); ++i)
	{
		vec[i]->setTitle(show_extensions);
		tabs->setTabText(i, vec[i]->title());
	}
}

//Saving all notes
void NoteList::SaveAll()
{
	for(int i=0; i<vec.size(); ++i)
	{
		vec[i]->save(true); //Forced saving
	}
}
