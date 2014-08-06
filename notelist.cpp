#include <QFileSystemWatcher>
#include <QFileDialog>
#include <QInputDialog>
#include <QMessageBox>
#include <QPrinter>
#include <QTextEdit>

#include "notelist.h"
#include "settings.h"

#include "textnote.h"
#include "htmlnote.h"
#include "picturenote.h"
#ifdef NOTE_TODO_FORMAT
    #include "todonote.h"
#endif
#ifdef NOTE_XML_FORMAT
	#include "note_xml.h"
#endif

NoteList::NoteList(QWidget* parent)
    : QObject(), vec(), history_index(0), history_forward_pressed(false),
      history_back_pressed(false), current_index(-1)
{
    initNoteTypes();

    tabs = new ZTabWidget(parent);
    tabs->setDocumentMode(true);
    tabs->setTabPosition(QTabWidget::TabPosition(settings.getTabPosition()));

    //Setting and testing directory
    dir.setPath(settings.getNotesPath());
    if(!dir.exists()) if(!dir.mkpath(dir.path())) dir.setPath("");
    while(dir.path().isEmpty())
    {
        dir.setPath(QFileDialog::getExistingDirectory(0,
            tr("Select place for notes directory"), QDir::homePath(),
            QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks));
        if(!dir.path().isEmpty())
        {
            QString new_path = QDir(dir.path()+'/'+tr("Notes")).absolutePath();
            settings.setNotesPath(new_path);
            if(!dir.exists()) if(!dir.mkpath(dir.path())) dir.setPath("");
            if(!dir.isReadable()) dir.setPath("");
        }
    }

    //Loading files' list
    if(settings.getShowHidden()) dir.setFilter(QDir::Files | QDir::Hidden | QDir::Readable);
    else dir.setFilter(QDir::Files | QDir::Readable);
    QFileInfoList flist = dir.entryInfoList();
    const QString& last_note_title = settings.getLastNote();
    int old_index=-1;
    for(int i=0; i<flist.size(); ++i)
    {
        //Loading note
        Note* note = this->add(flist.at(i), false);
        if(old_index==-1 && note->fileName()==last_note_title) old_index = i;
    }
    if(old_index!=-1) tabs->setCurrentIndex(old_index);
    else if(empty()) create(Note::type_html);
    current_index = tabs->currentIndex();

    watcher = new QFileSystemWatcher(this);
    watcher->addPath(dir.absolutePath());

    connect(tabs, SIGNAL(currentChanged(int)), this, SLOT(currentTabChanged(int)));
    connect(&settings, SIGNAL(ShowExtensionsChanged(bool)), this, SLOT(showExtensionsChanged(bool)));
    connect(&settings, SIGNAL(TabPositionChanged()), this, SLOT(tabPositionChanged()));
    connect(&settings, SIGNAL(NotesPathChanged()), this, SLOT(notesPathChanged()));
    connect(&settings, SIGNAL(ShowHiddenChanged()), this, SLOT(scanForNewFiles()));
    connect(watcher, SIGNAL(directoryChanged(QString)), this, SLOT(scanForNewFiles()));

    connect(tabs, SIGNAL(tabBarDoubleClickedOnTab(int)), this, SLOT(changeNoteName(int)));

    // Save a parent widget pointer.
    parentWidget = parent;
}

NoteList::~NoteList()
{
    tabs->clear();
}

void NoteList::initNoteTypes()
{
    registerType(Note::type_text, QObject::tr("Text Note"),
                 QObject::tr("Simple text Note"), ":/res/note_types/txt.png",
                 ":/res/note_types/16/txt.png", "txt,");
    registerType(Note::type_html, QObject::tr("HTML Note"),
                 QObject::tr("Simple Note with text formating"), ":/res/note_types/html.png",
                 ":/res/note_types/16/html.png", "html,htm");
    registerType(Note::type_picture, QObject::tr("Picture Note"), QObject::tr("Simple text Note"),
                 ":/res/note_types/pic.png", ":/res/note_types/16/pic.png", "png,jpeg,jpg,png,gif",
                 false);
#ifdef NOTE_TODO_FORMAT
    registerType(Note::type_todo, QObject::tr("TODO Note"), QObject::tr("Simple TODO list"),
                 ":/res/note_types/todo.png", ":/res/note_types/16/todo.png", "ztodo");
#endif
#ifdef NOTE_XML_FORMAT
    registerType(Note::type_xml, QObject::tr("XML Note"), QObject::tr("XML file"),
                 ":/res/note_types/xml.png", ":/res/note_types/16/xml.png", "xml");
#endif
}

void NoteList::registerType(Note::Type id, const QString& title,
					const QString& description, const QString& big_icon_path,
					const QString& small_icon_path, const QString extensions,
					bool visible)
{
	note_types[id] = NoteType(id, title, description, big_icon_path, small_icon_path, extensions, visible);
	foreach(const QString& extension, note_types[id].extensions()) NOTE_TYPE_MAP[extension] = id;
}

void NoteList::scanForNewFiles()
{
	if(settings.getShowHidden()) dir.setFilter(QDir::Files | QDir::Hidden | QDir::Readable);
	else dir.setFilter(QDir::Files | QDir::Readable);
	dir.refresh();
	QFileInfoList flist = dir.entryInfoList();
	for(int i=0; i<flist.size(); ++i)
	{
		//Loading note
		const QString& filename = flist.at(i).fileName();
		bool exists = notes_filenames.contains(filename);
		if(!exists) add(flist.at(i));
    }
}

void NoteList::changeNoteName(int tabIndex)
{
    Q_UNUSED(tabIndex)
    renameCurrentNote();
}

Note::Type NoteList::getType(const QFileInfo& fileinfo) const
{
	return NOTE_TYPE_MAP[fileinfo.suffix().toLower()];
}

void NoteList::create(Note::Type type, const QString& name)
{
	QString extension = noteTypes()[type].defaultExtension();
	QString filename = QString("%1.%2").arg(name.isEmpty()?"0":name).arg(extension);
	QFile file(dir.absoluteFilePath(filename));
	int n = 0;
	while(file.exists()) //Searching for free filename
	{
		filename = QString("%1.%2").arg(++n).arg(extension);
		file.setFileName(dir.absoluteFilePath(filename));
	}
	add(file);
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

void NoteList::remove(int i)
{
	disconnect(tabs, SIGNAL(currentChanged(int)), this, SLOT(currentTabChanged(int)));
	Note* note = vec.takeAt(i);
	tabs->removeTab(i);
	QString filename = note->fileName();
	note->deleteLater();
	current_index = tabs->currentIndex();
	connect(tabs, SIGNAL(currentChanged(int)), this, SLOT(currentTabChanged(int)));
	emit currentNoteChanged(-1, current_index);
	notes_filenames.remove(filename);
}

void NoteList::move(const QString& path)
{
	Note* note;
	foreach (note, vec)
	{
		note->move(path);
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

void NoteList::removeCurrentNote()
{
	Note* note = current();
	if(!note) return;
    QMessageBox msgBox(QMessageBox::Question,
                       tr("Delete Note"),
                       tr("Do you really want to delete note %1 ?").arg(note->title()),
                       QMessageBox::Yes | QMessageBox::No,
                       parentWidget);
	int ret = msgBox.exec();
	if(ret == QMessageBox::Yes)
	{
		remove(currentIndex());
	}
}

void NoteList::renameCurrentNote()
{
	if(empty()) return;
	Note* note = current();
	if(!note) return;
	bool ok;

	int typedot = note->fileName().lastIndexOf(".");
	QString filename = note->fileName();
	QString extension;
	if(typedot != -1)
	{
		// Specify size to keep it backwards compitable with Qt >4.7
		extension = QString(&note->fileName().data()[typedot], note->fileName().length() - typedot);
		filename.truncate(typedot);
	}

    QString new_name = QInputDialog::getText(parentWidget,
                                             tr("Rename note"),
                                             tr("New name:"),
                                             QLineEdit::Normal, filename,  &ok);
	if(ok && !new_name.isEmpty())
	{
		new_name.append(extension);
		QFile file(dir.absoluteFilePath(new_name));
        if(!file.exists())
            rename(current_index, new_name);
		else
		{
            QMessageBox::information(parentWidget,
                                     tr("Note renaming"),
                                     tr("Note %1 already exists!").arg(new_name));
		}
	}
}

void NoteList::rename(int index, const QString& title)
{
	Note* note = vec.at(index);
	note->save();
	notes_filenames.remove(note->fileName());
	note->rename(title);
	tabs->setTabText(index, note->title());
	notes_filenames.insert(note->fileName());
}

void NoteList::currentTabChanged(int index)
{
	if(index==-1) return;
	if(current_index!=-1) if(current()) current()->save();
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

void NoteList::showExtensionsChanged(bool show_extensions)
{
	for(int i=0; i<vec.size(); ++i)
	{
		vec[i]->updateTitle(show_extensions);
		tabs->setTabText(i, vec[i]->title());
	}
}

void NoteList::tabPositionChanged()
{
	tabs->setTabPosition(QTabWidget::TabPosition(settings.getTabPosition()));
}

//Saving all notes
void NoteList::saveAll()
{
	Note* note;
	foreach (note, vec)
	{
		note->save(true); //Forced saving
	}
}

void NoteList::notesPathChanged()
{
    QMessageBox msgBox(QMessageBox::Question,
                       tr("Move notes"),
                       tr("notes path changed!\nDo you want to move your notes to new place ?"),
                       QMessageBox::Yes | QMessageBox::No,
                       parentWidget);
	int ret = msgBox.exec();
	if(ret == QMessageBox::Yes)
	{
		dir.setPath(settings.getNotesPath());
		QString dir_path = dir.absolutePath();
		move(dir_path);
	}
    else
        QMessageBox::information(parentWidget, tr("notes path change"),
                                 tr("You need restart application to get effect."));
}

void NoteList::retranslate(const QLocale& locale)
{
	Note* note;
	foreach (note, vec)
	{
		note->retranslate(locale);
	}
}
