#include "settings.h"
#include "toolbaraction.h"

#include <QObject>
#include <QApplication>
#include <QLibraryInfo>
#include <QDir>

Settings::Settings() : config("pDev", "zNotes")
{
}

/*
  Loading settings...
*/

void Settings::load()
{
	if(config.allKeys().size()>0) //if exist - reading settings
	{
		notes_path = config.value("NotesPath").toString();
		last_note = config.value("LastNote").toString();
		hide_start = config.value("HideStart").toBool();
		//
		dialog_geometry = config.value("DialogGeometry").toByteArray();
		dialog_state = config.value("DialogState").toByteArray();
		//
		tab_position = TabPosition(config.value("TabPosition").toInt());
		show_hidden = config.value("ShowHidden").toBool();
		show_extensions = config.value("ShowExtensions").toBool();
		//
		hide_frame = config.value("HideFrame").toBool();
		stay_top = config.value("StayTop").toBool();
		//
		single_instance = config.value("SingleInstance").toBool();
		copy_start_raise = config.value("CopyStartRaise").toBool();
		//
		file_scanner = config.value("FileScanner").toBool();
		file_scanner_timeout = config.value("FileScannerTimeout").toInt();
		//
		note_font.fromString(config.value("NoteFont").toString());
		note_links_highlight = config.value("NoteLinksHighlight").toBool();
		note_links_open = config.value("NoteLinksOpen").toBool();
		//
		int ScriptCount = config.value("ComandCount").toInt();
		for(int i=0; i<ScriptCount; ++i)
		{
			script_model.append(
				config.value(QString("ComandName%1").arg(i)).toString(),
				config.value(QString("ComandFile%1").arg(i)).toString(),
				config.value(QString("ComandIcon%1").arg(i)).toString());
		}
		script_show_output = config.value("ScriptShowOutput").toBool();
		script_copy_output = config.value("ScriptCopyOutput").toBool();
		//
		language_custom = config.value("LanguageCustom").toBool();
		locale_current = QLocale(config.value("LanguageCurrent").toString());
		//
		if(config.contains("Toolbar/itemCount"))
		{
			tb_items.resize(config.value("Toolbar/itemCount").toInt());
			for(int i=itemAdd; i<itemMax; ++i)
			{
				int pos = config.value(ToolbarAction(item_enum(i)).pref_name(), tb_items.size()).toInt();
				if(pos<tb_items.size()) tb_items[pos] = i; //Item's position
			}
		}
	}
	/*
	* If settings don't exist - setup default settings
	*/
	//Setting default path to notes
	if(notes_path.isEmpty())
	{
#ifdef Q_WS_X11
		notes_path = QDir::homePath()+"/.local/share/notes";
#else
		if(!QDir::homePath().isEmpty()) notes_path = QDir::homePath()+"/Notes");
#endif
		config.setValue("NotesPath", notes_path);
	}
	//Settings single instance options
	if(!config.contains("SingleInstance"))
	{
		single_instance = true;
		config.setValue("SingleInstance", single_instance);
		copy_start_raise = true;
		config.setValue("CopyStartRaise", copy_start_raise);
	}
	//Setting default note options
	if(!config.contains("NoteLinksHighlight"))
	{
		note_links_highlight = true;
		config.setValue("NoteLinksHighlight", note_links_highlight);
	}
	if(!config.contains("NoteLinksOpen"))
	{
		note_links_open = true;
		config.setValue("NoteLinksOpen", note_links_open);
	}
	//Setting default scripts
	if((script_model.rowCount()==0) && !config.contains("ComandCount"))
	{
		script_model.append("Print note's content", "cat", "");
		script_model.append("Upload to pasterbin", "/usr/bin/pastebin", "");
		config.setValue("ComandCount", script_model.rowCount());
		for(int i=0; i<script_model.rowCount(); ++i)
		{
			config.setValue(QString("ComandName%1").arg(i), script_model.getName(i));
			config.setValue(QString("ComandFile%1").arg(i), script_model.getFile(i));
			config.setValue(QString("ComandIcon%1").arg(i), script_model.getIcon(i));
		}
	}
	//Setting default filescanner timeout
	if(!config.contains("FileScannerTimeout"))
	{
		file_scanner_timeout = 500;//500 msec
		config.setValue("FileScannerTimeout", file_scanner_timeout);
	}
	//Setting default tab position
	if(!config.contains("TabPosition"))
	{
		tab_position = West;
		config.setValue("TabPosition", tab_position);
	}
	//Setting default toolbar items
	if((tb_items.size()==0) && !config.contains("Toolbar/itemCount"))
	{
		tb_items.append(itemAdd);
		tb_items.append(itemRemove);
		tb_items.append(itemRename);
		tb_items.append(itemSeparator);
		tb_items.append(itemPrev);
		tb_items.append(itemNext);
		tb_items.append(itemSeparator);
		tb_items.append(itemCopy);
		tb_items.append(itemSeparator);
		tb_items.append(itemSetup);
		tb_items.append(itemInfo);
		tb_items.append(itemSeparator);
		tb_items.append(itemRun);
		tb_items.append(itemSearch);
		tb_items.append(itemSeparator);
		tb_items.append(itemExit);
		config.setValue("Toolbar/itemCount", tb_items.size());
		for(int i=0; i<tb_items.size(); ++i)
			if(tb_items[i]!=itemSeparator)
				config.setValue(ToolbarAction(item_enum(tb_items[i])).pref_name(), i);
	}
	loadLanguages();
	//
#ifdef unix
	//Fixing Qt's problem on unix systems...
	QString system_lang(qgetenv("LANG").constData());
	system_lang.truncate(system_lang.indexOf('.'));
	if(system_lang.size()>0) locale_system = QLocale(system_lang);
	else locale_system = QLocale::system().language();
#else
	locale_system = QLocale::system().language();
#endif
	QLocale locale = (language_custom)?locale_current:locale_system;

	QMap<int, QMap<int, QString> >::const_iterator it = translations.find(locale.language());
	if(it!=translations.end()) //if translation list has locale language
	{
		const QMap<int, QString>& country_list = it.value();
		if(!country_list.contains(locale.country()))
		{
			QList<QLocale::Country> language_countries = QLocale::countriesForLanguage(locale.language());
			if(!language_countries.empty() && country_list.contains(language_countries[0]))
			{
				QLocale::Country country = language_countries[0];
				locale = QLocale(locale.language(), country);
			}
			else if(!country_list.empty())
			{
				QLocale::Country country = QLocale::Country(country_list.begin().key());
				locale = QLocale(locale.language(), country);
			}
			else locale = QLocale::c();
		}
	}
	else locale = QLocale::c();

	setLocale(locale);

	qApp->installTranslator(&qtranslator);
	qApp->installTranslator(&translator);
}

/*
  Loading list of qm-files...
*/

void Settings::loadLanguages()
{
	//adding translations search paths
	QStringList translation_dirs;
	translation_dirs << QCoreApplication::applicationDirPath();
	translation_dirs << QCoreApplication::applicationDirPath()+"/translations";
#ifdef PROGRAM_DATA_DIR
	translation_dirs << QString(PROGRAM_DATA_DIR)+"/translations";
#endif
#ifdef Q_WS_MAC
	translation_dirs << QCoreApplication::applicationDirPath()+"/../Resources";
#endif
	//Setting default(English) translation path
	translations[QLocale::English][QLocale::UnitedStates]="";
	//looking for qm-files in translation directories
	QStringListIterator dir_path(translation_dirs);
	while(dir_path.hasNext())
	{
		QDir dir(dir_path.next());
		QStringList fileNames = dir.entryList(QStringList("znotes_*.qm"));
		for(int i=0; i < fileNames.size(); ++i)
		{
			QString filename(fileNames[i]);
			QString fullpath(dir.absoluteFilePath(filename));
			filename.remove(0, filename.indexOf('_') + 1);
			filename.chop(3);
			QLocale locale(filename);
			if(!translations[locale.language()].contains(locale.country()))
			{
				translations[locale.language()][locale.country()]=fullpath;
			}
		}
	}
}

/*
  Saving notes's path
*/
void Settings::setNotesPath(const QString& path)
{
	if(notes_path != path)
	{
		notes_path = path;
		config.setValue("NotesPath", notes_path);
		emit NotesPathChanged();
	}
}

/*
  Saving title of last note
*/
void Settings::setLastNote(const QString& note_name)
{
	if(last_note != note_name)
	{
		last_note = note_name;
		config.setValue("LastNote", last_note);
	}
}

/*
  Saving option (hiding dialog on start)
*/
void Settings::setHideStart(bool hide)
{
	if(hide_start != hide)
	{
		hide_start = hide;
		config.setValue("HideStart", hide_start);
	}
}

void Settings::setShowHidden(bool v)
{
	if(show_hidden != v)
	{
		show_hidden = v;
		config.setValue("ShowHidden", show_hidden);
		emit ShowHiddenChanged();
	}
}

void Settings::setShowExtensions(bool v)
{
	if(show_extensions != v)
	{
		show_extensions = v;
		config.setValue("ShowExtensions", show_extensions);
		emit ShowExtensionsChanged(show_extensions);
	}
}

/*
  Saving option (hiding window decoration)
*/
void Settings::setHideFrame(bool v)
{
	if(hide_frame != v)
	{
		hide_frame = v;
		config.setValue("HideFrame", hide_frame);
		emit WindowStateChanged();
	}
}

/*
  Saving option (staying on top)
*/
void Settings::setStayTop(bool v)
{
	if(stay_top != v)
	{
		stay_top = v;
		config.setValue("StayTop", stay_top);
		emit WindowStateChanged();
	}
}

void Settings::setSingleInstance(bool v)
{
	if(single_instance != v)
	{
		single_instance = v;
		config.setValue("SingleInstance", single_instance);
	}
}

void Settings::setCopyStartRaise(bool v)
{
	if(copy_start_raise != v)
	{
		copy_start_raise = v;
		config.setValue("CopyStartRaise", copy_start_raise);
	}
}

/*
  Saving option (scanning for new files)
*/
void setFileScanner(bool v);

/*
  Saving option (file scan timeout)
*/
void Settings::setFileScanner(bool v)
{
	if(file_scanner != v)
	{
		file_scanner = v;
		config.setValue("FileScanner", file_scanner);
		emit FileScannerEnChanged(file_scanner);
	}
}

/*
  Saving option (hiding window decoration)
*/
void Settings::setFileScannerTimeout(int v)
{
	if(file_scanner_timeout != v)
	{
		file_scanner_timeout = v;
		config.setValue("FileScannerTimeout", file_scanner_timeout);
		emit FileScannerTimeoutChanged(file_scanner_timeout);
	}
}

/*
  Saving dialog's params
*/
void Settings::setDialogGeometry(const QByteArray& v)
{
	dialog_geometry = v;
	config.setValue("DialogGeometry", dialog_geometry);
}
void Settings::setDialogState(const QByteArray& v)
{
	dialog_state = v;
	config.setValue("DialogState", dialog_state);
}

/*
  Setting tabs position
*/
void Settings::setTabPosition(TabPosition v)
{
	tab_position = v;
	config.setValue("TabPosition", tab_position);
	emit TabPositionChanged();
}

/*
  Saving notes's font
*/
void Settings::setNoteFont(const QFont& v)
{
	if(note_font != v)
	{
		note_font = v;
		config.setValue("NoteFont", note_font.toString());
		emit NoteFontChanged();
	}
}

/*
  Saving notes's links highlight option
*/
void Settings::setNoteLinksHighlight(bool v)
{
	if(note_links_highlight != v)
	{
		note_links_highlight = v;
		config.setValue("NoteLinksHighlight", note_links_highlight);
		emit NoteHighlightChanged();
	}
}

/*
  Saving notes's links highlight option
*/
void Settings::setNoteLinksOpen(bool v)
{
	if(note_links_open != v)
	{
		note_links_open = v;
		config.setValue("NoteLinksOpen", note_links_open);
		emit NoteLinkOpenChanged();
	}
}

/*
  Saving script's options
*/
void Settings::setScriptShowOutput(bool v)
{
	if(script_show_output != v)
	{
		script_show_output = v;
		config.setValue("ScriptShowOutput", script_show_output);
	}
}
void Settings::setScriptCopyOutput(bool v)
{
	if(script_copy_output != v)
	{
		script_copy_output = v;
		config.setValue("ScriptCopyOutput", script_copy_output);
	}
}

/*
  Saving scripts
*/
void Settings::setScripts()
{
	config.setValue("ComandCount", script_model.rowCount());
	for(int i=0; i<script_model.rowCount(); ++i)
	{
		config.setValue(QString("ComandName%1").arg(i), script_model.getName(i));
		config.setValue(QString("ComandFile%1").arg(i), script_model.getFile(i));
		config.setValue(QString("ComandIcon%1").arg(i), script_model.getIcon(i));
	}
}

/*
  Saving toolbar's items
*/
void Settings::setToolbarItems(const QVector<int>& v)
{
	if(v==tb_items) return;
	//removing old settings
	for(int i=0; i<tb_items.size(); ++i) if(tb_items[i]!=itemSeparator)
	{
		config.remove(ToolbarAction(item_enum(tb_items[i])).pref_name()); //dirty hack =(
	}
	tb_items = v;
	//saving settings
	config.setValue("Toolbar/itemCount", tb_items.size());
	for(int i=0; i<tb_items.size(); ++i) if(tb_items[i]!=itemSeparator)
	{
		config.setValue(ToolbarAction(item_enum(tb_items[i])).pref_name(), i);
	}
	emit ToolbarItemsChanged();
}

/*
  Saving current language
*/
void Settings::setLocaleCurrent(const QLocale& locale)
{
	if(locale_current != locale)
	{
		locale_current = locale;
		config.setValue("LanguageCurrent", locale_current.name());
	}
	if(language_custom) setLocale(locale_current);
}

/*
  Saving option of using custom language
*/
void Settings::setLocaleCustom(bool v)
{
	if(language_custom != v)
	{
		language_custom = v;
		config.setValue("LanguageCustom", language_custom);
		if(!language_custom) setLocale(locale_system);
	}
}

/*
  Setting language
*/
void Settings::setLocale(const QLocale& locale)
{
	qtranslator.load("qt_"+locale.name(), QLibraryInfo::location(QLibraryInfo::TranslationsPath));
	translator.load(translations[locale.language()][locale.country()]);
}
