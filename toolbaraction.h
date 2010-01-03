#ifndef TOOLBARACTION_H
#define TOOLBARACTION_H

#include <QString>
#include <QIcon>
#include <QObject>

enum item_enum
{
	itemSeparator,
	itemAdd,
	itemRemove,
	itemRename,
	itemPrev,
	itemNext,
	itemCopy,
	itemSetup,
	itemInfo,
	itemRun,
	itemSearch,
	itemExit,
	itemFormatBold,
	itemFormatItalic,
	itemFormatStrikeout,
	itemFormatUnderline,
	itemMax
};

class ToolbarAction
{
public:
	ToolbarAction(item_enum id):item_id(id) {}
	inline QString text()
	{
		switch(item_id)
		{
			case itemSeparator:	return QObject::tr("Separator");
			case itemAdd:		return QObject::tr("Create new note");
			case itemRemove:	return QObject::tr("Remove this note");
			case itemRename:	return QObject::tr("Rename this note");
			case itemPrev:		return QObject::tr("Prev note");
			case itemNext:		return QObject::tr("Next note");
			case itemCopy:		return QObject::tr("Copy this note to clipboard");
			case itemSetup:		return QObject::tr("Preferences");
			case itemInfo:		return QObject::tr("Info");
			case itemRun:		return QObject::tr("Commands");
			case itemSearch:	return QObject::tr("Search");
			case itemExit:		return QObject::tr("Exit");
			case itemFormatBold:		return QObject::tr("Bold");
			case itemFormatItalic:		return QObject::tr("Italic");
			case itemFormatStrikeout:	return QObject::tr("Strikeout");
			case itemFormatUnderline:	return QObject::tr("Underline");
			default: return 0;
		}
	}
	inline QIcon icon()
	{
		switch(item_id)
		{
			case itemAdd:		return QIcon(":/res/add.png");
			case itemRemove:	return QIcon(":/res/remove.png");
			case itemRename:	return QIcon(":/res/rename.png");
			case itemPrev:		return QIcon(":/res/prev.png");
			case itemNext:		return QIcon(":/res/next.png");
			case itemCopy:		return QIcon(":/res/copy.png");
			case itemSetup:		return QIcon(":/res/settings.png");
			case itemInfo:		return QIcon(":/res/info.png");
			case itemRun:		return QIcon(":/res/exec.png");
			case itemSearch:	return QIcon(":/res/find.png");
			case itemExit:		return QIcon(":/res/exit.png");
			case itemFormatBold:		return QIcon(":/res/format_bold.png");
			case itemFormatItalic:		return QIcon(":/res/format_italic.png");
			case itemFormatStrikeout:	return QIcon(":/res/format_strikeout.png");
			case itemFormatUnderline:	return QIcon(":/res/format_underline.png");
			default: return QIcon();
		}
	}
	inline bool isSeparator() { return item_id==itemSeparator; }
private:
	item_enum item_id;
};

#endif // TOOLBARACTION_H
