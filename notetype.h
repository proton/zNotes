#ifndef NOTETYPE_H
#define NOTETYPE_H

#include <QIcon>
#include <QStringList>

#include "note.h"

class NoteType
{
public:
	NoteType() {}
	NoteType(Note::Type id, const QString& title, const QString& description,
			const QString& big_icon_path, const QString& small_icon_path,
			const QString extensions, bool visible = true);

	inline Note::Type id() const { return note_id; }
	inline const QString& title() const { return note_title; }
	inline const QString& description() const { return note_description; }
	inline const QIcon& icon() const { return big_icon; }
	inline const QIcon& smallIcon() const { return small_icon; }
	inline bool visible() const { return is_visible; }
	inline const QStringList& extensions() const { return exts; }
	inline const QString defaultExtension() const { return exts.isEmpty()?"":exts.first(); }

private:
	Note::Type note_id;
	QStringList exts;
	QString note_title;
	QString note_description;
	QIcon big_icon;
	QIcon small_icon;
	bool is_visible;
};

#endif // NOTETYPE_H
