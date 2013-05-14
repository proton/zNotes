#include "notetype.h"

NoteType::NoteType(Note::Type id, const QString& title, const QString& description,
					const QString& big_icon_path, const QString& small_icon_path,
					const QString extensions, bool visible)
	: note_id(id),
	  note_title(title),
	  note_description(description),
	  big_icon(big_icon_path),
	  small_icon(small_icon_path),
	  is_visible(visible)
{
	exts = extensions.split(',');
}
