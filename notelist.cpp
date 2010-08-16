#include "notelist.h"
#include "settings.h"

#include "note_text.h"
#include "note_html.h"
#include "note_picture.h"
#ifdef NOTE_TODO_FORMAT
	#include "note_todo.h"
#endif
#ifdef NOTE_XML_FORMAT
	#include "note_xml.h"
#endif

NoteList::NoteList(QWidget* parent)
	: QObject(), vec(), history_index(0), current_index(-1), history_forward_pressed(false), history_back_pressed(false)
{
	//NOTE_TYPE_MAP init
	NOTE_TYPE_MAP[""] = Note::type_text;
	NOTE_TYPE_MAP["txt"] = Note::type_text;
	NOTE_TYPE_MAP["htm"] = Note::type_html;
	NOTE_TYPE_MAP["html"] = Note::type_html;
	NOTE_TYPE_MAP["jpg"] = Note::type_picture;
	NOTE_TYPE_MAP["jpeg"] = Note::type_picture;
	NOTE_TYPE_MAP["bmp"] = Note::type_picture;
	NOTE_TYPE_MAP["gif"] = Note::type_picture;
	NOTE_TYPE_MAP["png"] = Note::type_picture;
#ifdef NOTE_TODO_FORMAT
	NOTE_TYPE_MAP["ztodo"] = Note::type_todo;
#endif
#ifdef NOTE_XML_FORMAT
	NOTE_TYPE_MAP["xml"] = Note::type_xml;
#endif

	tabs = new QTabWidget(parent);
	tabs->setDocumentMode(true);
	tabs->setTabPosition(QTabWidget::TabPosition(settings.getTabPosition()));
	connect(tabs, SIGNAL(currentChanged(int)), this, SLOT(CurrentTabChanged(int)));
	connect(&settings, SIGNAL(ShowExtensionsChanged(bool)), this, SLOT(ShowExtensionsChanged(bool)));
	connect(&settings, SIGNAL(TabPositionChanged()), this, SLOT(TabPositionChanged()));
}

NoteList::~NoteList()
{
	tabs->clear();
}

Note::Type NoteList::getType(const QFileInfo& fileinfo) const
{
	return NOTE_TYPE_MAP[fileinfo.suffix().toLower()];
}

Note* NoteList::add(const QFileInfo& fileinfo, bool set_current)
{
	Note* note;
	Note::Type type = getType(fileinfo);
	switch(type)
	{
		case Note::type_text: note = new TextNote(fileinfo, type); break;
		case Note::type_html: note = new HtmlNote(fileinfo, type); break;
		case Note::type_picture: note = new PictureNote(fileinfo, type); break;
#ifdef NOTE_TODO_FORMAT
		case Note::type_todo: note = new TodoNote(fileinfo, type); break;
#endif
#ifdef NOTE_XML_FORMAT
		case Note::type_xml: note = new XmlNote(fileinfo, type); break;
#endif
		default: note = new TextNote(fileinfo, type); break;
	}

	vec.append(note);
	tabs->addTab(note->widget(), note->title());
	notes_filenames.insert(fileinfo.fileName());
	if(set_current) tabs->setCurrentWidget(note->widget());
	return note;
}

bool NoteList::load(const QFileInfo& fileinfo, const QString& old_title)
{
	Note* note = add(fileinfo, false);
	return (note->fileName()==old_title);
}

void NoteList::remove(int i)
{
	Note* note = vec[i];
	tabs->removeTab(i);
	vec.remove(i);
	QString filename = note->fileName();
	delete note;
	notes_filenames.remove(filename);
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
	//If find nothing
	//current()->widget()->setFocus();
}

void NoteList::rename(int index, const QString& title)
{
	Note* note = vec[index];
	note->save();
	notes_filenames.remove(note->fileName());
	note->rename(title);
	tabs->setTabText(index, note->title());
	notes_filenames.insert(note->fileName());
}

#include <QtDebug>

void NoteList::CurrentTabChanged(int index)
{
	if(index==-1) return;
	if(current_index!=-1) current()->save();
	int old_index = current_index;
	current_index = index;
	const QString path = vec[current_index]->absolutePath();
	//
	if(history_forward_pressed)
	{
		history_forward_pressed = false;
		if(history_index>0)
		{
			for(int i=history_index+1; i<history.size(); ++i)
			{
				if(path==history.at(i))
				{
					history_index = i;
					break;
				}
			}
		}
	}
	else if(history_back_pressed)
	{
		history_back_pressed = false;
		if(history_index>0)
		{
			for(int i=history_index-1; i>=0; --i)
			{
				if(path==history.at(i))
				{
					history_index = i;
					break;
				}
			}
		}
	}
	else
	{
		if((history.size()==0) && (old_index==-1))
		{
			history.append(path);
		}
		else
		{
			while(history.size()>(history_index+1)) history.removeLast();
			history.append(path);
			++history_index;
		}
	}
	//
	emit currentNoteChanged(old_index, current_index);
}

void NoteList::historyBack()
{
	if(!historyHasBack()) return;
	for(int j=history_index-1; j>=0; --j)
	{
		const QString& note_fname = history.at(j);
		for(int i=0; i<vec.size(); ++i)
		{
			if(vec[i]->absolutePath()==note_fname)
			{
				history_back_pressed = true;
				tabs->setCurrentIndex(i);
				return;
			}
		}
	}
}

void NoteList::historyForward()
{
	if(!historyHasForward()) return;
	for(int j=history_index+1; j<history.size(); ++j)
	{
		const QString& note_fname = history.at(j);
		for(int i=0; i<vec.size(); ++i)
		{
			if(vec[i]->absolutePath()==note_fname)
			{
				history_forward_pressed = true;
				tabs->setCurrentIndex(i);
				return;
			}
		}
	}
}

void NoteList::ShowExtensionsChanged(bool show_extensions)
{
	for(int i=0; i<vec.size(); ++i)
	{
		vec[i]->setTitle(show_extensions);
		tabs->setTabText(i, vec[i]->title());
	}
}

void NoteList::TabPositionChanged()
{
	tabs->setTabPosition(QTabWidget::TabPosition(settings.getTabPosition()));
}

//Saving all notes
void NoteList::SaveAll()
{
	for(int i=0; i<vec.size(); ++i)
	{
		vec[i]->save(true); //Forced saving
	}
}
