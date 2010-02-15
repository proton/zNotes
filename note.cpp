#include "note.h"
#include "settings.h"

#include <QTextStream>
#include <QClipboard>
#include <QApplication>
#include <QDir>

Note::Note(const QFileInfo& fileinfo, Note::Type type_new)
	: _type(type_new),
	file_info(fileinfo),
	content_changed(false)
{
	setTitle(settings.getShowExtensions());
}

Note::~Note()
{
}

//Setting note title
void Note::setTitle(bool show_extensions)
{
	file.setFileName(file_info.absoluteFilePath());
	_title = (show_extensions || (file_info.fileName()[0]=='.'))?
		file_info.fileName() : file_info.baseName();
}

//Renaming note
void Note::rename(const QString& new_name)
{
	file.close();
	QString absolute_file_path = file_info.dir().absoluteFilePath(new_name);
	file.rename(absolute_file_path);
	file_info.setFile(file);
	setTitle(settings.getShowExtensions());
}

//Moving note to another folder
void Note::move(const QString& new_dirname)
{
	file.close();
	QString filename = file_info.fileName();
	QString absolute_file_path = QDir(new_dirname).absoluteFilePath(filename);
	file.rename(absolute_file_path);
	file_info.setFile(file);
}

//Removing note
bool Note::remove()
{
	file.close();
	return file.remove();
}

//Searching in a note's content
bool Note::find(const QString& text, bool next)
{
	Q_UNUSED(text)
	Q_UNUSED(next)
	return false;
}

//------------------------------------------------------------------------------

void Note::contentChanged()
{
	content_changed = true;
}
