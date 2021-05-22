#ifndef TOOLBARACTION_H
#define TOOLBARACTION_H

#include <QString>
#include <QIcon>
#include <QObject>

enum item_enum
{
	itemSeparator,
	itemAdd,
	itemAddText,
	itemAddHtml,
	itemAddTodo,
	itemRemove,
	itemRename,
	itemBack,
	itemForward,
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
	itemFormatColor,
    itemExportPdf,
    itemPrintNote,
    itemPrintPreviewNote,
	itemMax
};

class ToolbarAction
{
public:
	ToolbarAction(item_enum id):item_id(id) {}
    // Returning action's title
	inline const QString text() const
	{
		switch(item_id)
		{
			case itemSeparator:	return QObject::tr("Separator");
			case itemAdd:		return QObject::tr("Create new note");
			case itemAddText:	return QObject::tr("Create new Text note");
			case itemAddHtml:	return QObject::tr("Create new HTML note");
			case itemAddTodo:	return QObject::tr("Create new TODO note");
			case itemRemove:	return QObject::tr("Remove this note");
			case itemRename:	return QObject::tr("Rename this note");
			case itemBack:		return QObject::tr("Back");
			case itemForward:	return QObject::tr("Forward");
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
			case itemFormatColor:		return QObject::tr("Text color");
            case itemExportPdf:         return QObject::tr("Export note to PDF");
            case itemPrintNote:         return QObject::tr("Print...");
            case itemPrintPreviewNote:  return QObject::tr("Print Preview...");
			default: return 0;
		}
	}
    //Returning action's icon
    inline QIcon icon() const
    {
        switch(item_id)
        {
            case itemAdd:               return loadIcon("list-add");
            case itemAddText:           return loadIcon("list-add");
            case itemAddHtml:           return loadIcon("list-add");
            case itemAddTodo:           return loadIcon("list-add");
            case itemRemove:            return loadIcon("list-remove");
            case itemRename:            return loadIcon("edit-rename");
            case itemBack:              return loadIcon("go-previous");
            case itemForward:           return loadIcon("go-next");
            case itemPrev:              return loadIcon("go-previous-view");
            case itemNext:              return loadIcon("go-next-view");
            case itemCopy:              return loadIcon("edit-copy");
            case itemSetup:             return loadIcon("configure");
            case itemInfo:              return loadIcon("help-about");
            case itemRun:               return loadIcon("run-build");
            case itemSearch:            return loadIcon("edit-find");
            case itemExit:              return loadIcon("window-close");
            case itemFormatBold:		return loadIcon("format-text-bold");
            case itemFormatItalic:		return loadIcon("format-text-italic");
            case itemFormatStrikeout:	return loadIcon("format-text-strikethrough");
            case itemFormatUnderline:	return loadIcon("format-text-underline");
            case itemFormatColor:		return loadIcon("format-text-color");
            case itemExportPdf:         return loadIcon("application-pdf");
            case itemPrintNote:         return loadIcon("printer");
            case itemPrintPreviewNote:  return loadIcon("document-print-preview");
            default: return QIcon();
        }
    }
    inline QIcon loadIcon(const QString& file_name) const {
        auto fallback_icon = QIcon(":/res/toolbar/" + file_name + ".png");
        return fallback_icon;
        return QIcon::fromTheme(file_name, fallback_icon);
    }
	//Returning action's identifiactor in configuration file
	inline const QString pref_name() const
	{
		switch(item_id)
		{
			case itemAdd:		return "Toolbar/itemAdd";
			case itemAddText:	return "Toolbar/itemAddText";
			case itemAddHtml:	return "Toolbar/itemAddHtml";
			case itemAddTodo:	return "Toolbar/itemAddTodo";
			case itemRemove:	return "Toolbar/itemRemove";
			case itemRename:	return "Toolbar/itemRename";
			case itemBack:		return "Toolbar/itemBack";
			case itemForward:	return "Toolbar/itemForward";
			case itemPrev:		return "Toolbar/itemPrev";
			case itemNext:		return "Toolbar/itemNext";
			case itemCopy:		return "Toolbar/itemCopy";
			case itemSetup:		return "Toolbar/itemSetup";
			case itemInfo:		return "Toolbar/itemInfo";
			case itemRun:		return "Toolbar/itemRun";
			case itemSearch:	return "Toolbar/itemSearch";
			case itemExit:		return "Toolbar/itemExit";
			case itemFormatBold:		return "Toolbar/itemFormatBold";
			case itemFormatItalic:		return "Toolbar/itemFormatItalic";
			case itemFormatStrikeout:	return "Toolbar/itemFormatStrikeout";
			case itemFormatUnderline:	return "Toolbar/itemFormatUnderline";
			case itemFormatColor:		return "Toolbar/itemFormatColor";
            case itemExportPdf:	        return "Toolbar/itemExportPdf";
            case itemPrintNote:         return "Toolbar/itemPrint";
            case itemPrintPreviewNote:  return "Toolbar/itemPrintPreview";
			default:			return "";
		}
	}
    inline bool isSeparator() { return item_id == itemSeparator; }
	inline bool isCheckable()
	{
		switch(item_id)
		{
		case itemSearch:
		case itemFormatBold:
		case itemFormatItalic:
		case itemFormatStrikeout:
		case itemFormatUnderline:
			return true;
		default:
			return false;
		}
	}
	//Is item enabled when notelist is empty
	inline bool isEnabledWhenEmpty()
	{
		switch(item_id)
		{
		case itemAdd:
		case itemAddText:
		case itemAddHtml:
		case itemAddTodo:
			return true;
		default:
			return false;
		}
	}

private:
	item_enum item_id;
};

#endif // TOOLBARACTION_H
