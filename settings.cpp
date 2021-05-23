#include "settings.h"
#include "toolbaraction.h"

#include <QObject>
#include <QApplication>
#include <QLibraryInfo>
#include <QDir>

Settings::Settings()
	: config("pDev", "zNotes")
{
}

/*
  Loading settings...
*/

void Settings::load()
{
	if(config.allKeys().size()) //if exist - reading settings
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
		note_font.fromString(config.value("NoteFont").toString());
		note_links_open = config.value("NoteLinksOpen").toBool();
		note_paste_plaintext = config.value("NotePastePlaintext").toBool();
		//
		int ScriptCount = config.value("ComandCount").toInt();
		for(int i=0; i<ScriptCount; ++i)
		{
			script_model.append(
				config.value(QString("ComandName%1").arg(i)).toString(),
				config.value(QString("ComandFile%1").arg(i)).toString(),
				config.value(QString("ComandIcon%1").arg(i)).toString());
		}
		note_highlight = config.value("NoteHighlight").toBool();

		int HighlightRuleCount = config.beginReadArray("HighlightRules");
		highlight_rules.resize(HighlightRuleCount);
		for(int i=0; i<highlight_rules.size(); ++i)
		{
			config.setArrayIndex(i);
			highlight_rules[i].enabled = config.value("enabled").toBool();
			highlight_rules[i].regexp = config.value("regexp").toString();
			highlight_rules[i].color = QColor(config.value("color").toString());
		}
		config.endArray();

		script_show_output = config.value("ScriptShowOutput").toBool();
		script_copy_output = config.value("ScriptCopyOutput").toBool();
		//
		language_custom = config.value("LanguageCustom").toBool();
		locale_current = QLocale(config.value("LanguageCurrent").toString());
		//
		if(config.contains("Toolbar/itemCount"))
		{
			tb_items.resize(config.value("Toolbar/itemCount").toInt());
            for (int i = itemAdd; i < itemMax; ++i) {
				int pos = config.value(ToolbarAction(item_enum(i)).pref_name(), tb_items.size()).toInt();
                if (pos < tb_items.size())
                    tb_items[pos] = i; //Item's position
			}
		}

        icons_size = config.value("IconsSize").toInt();
        icons_use_system_theme = config.value("IconsUseSystemTheme").toBool();
	}
	/*
	* If settings don't exist - setup default settings
	*/
	//Setting default path to notes
	if(notes_path.isEmpty())
	{
#if defined Q_WS_X11 /* Qt4 */ || defined Q_OS_LINUX /* Qt5 */
	notes_path = QDir::homePath()+"/.local/share/notes";
#elif defined Q_WS_WIN /* Qt4 */ || defined Q_OS_WIN /* Qt5 */
	QSettings win_settings("Microsoft", "Windows");
	QString mydocuments_path = win_settings.value("CurrentVersion/Explorer/Shell Folders/Personal", "").toString();
	if(!mydocuments_path.isEmpty()) notes_path = mydocuments_path+"/Notes";
	else if(!QDir::homePath().isEmpty()) notes_path = QDir::homePath()+"/Notes";
#else
    if(!QDir::homePath().isEmpty()) notes_path = QDir::homePath()+"\\Notes";
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
	if(!config.contains("NoteLinksOpen"))
	{
		note_links_open = true;
		config.setValue("NoteLinksOpen", note_links_open);
	}
	if(!config.contains("NotePastePlaintext"))
	{
		note_paste_plaintext = false;
		config.setValue("NotePastePlaintext", note_paste_plaintext);
	}
	//Setting default highlight rules
	if(!config.contains("NoteHighlight"))
	{
		if(config.contains("NoteLinksHighlight"))
			note_highlight = config.value("NoteLinksHighlight").toBool();
		else
			note_highlight = true;

		config.setValue("NoteHighlight", note_highlight);
		highlight_rules.append(HighlightRule(true, "(http|https|ftp)://\\S+", QColor(Qt::blue)));
		highlight_rules.append(HighlightRule(false, "(\\d{1,3}\\.){3,3}\\d{1,3}(\\:\\d+)?", QColor("#500000")));
		highlight_rules.append(HighlightRule(false, "(0x|)\\d+", QColor("#147E16")));
		highlight_rules.append(HighlightRule(false, "#[0-9a-fA-F]{6,6}", QColor("#CEC51B")));

		config.beginWriteArray("HighlightRules", highlight_rules.size());
		for(int i=0; i<highlight_rules.size(); ++i)
		{
			config.setArrayIndex(i);
			config.setValue("enabled", highlight_rules.at(i).enabled);
			config.setValue("regexp", highlight_rules.at(i).regexp);
			config.setValue("color", highlight_rules.at(i).color);
		}
		config.endArray();
	}
	//Setting default scripts
	if((script_model.rowCount()==0) && !config.contains("ComandCount"))
	{
	#ifdef unix
		script_model.append("Print note's content", "cat", "");
    #elif defined Q_WS_WIN /* Qt4 */ || defined Q_OS_WIN /* Qt5 */
		//
	#endif
		config.setValue("ComandCount", script_model.rowCount());
		for(int i=0; i<script_model.rowCount(); ++i)
		{
			config.setValue(QString("ComandName%1").arg(i), script_model.getName(i));
			config.setValue(QString("ComandFile%1").arg(i), script_model.getFile(i));
			config.setValue(QString("ComandIcon%1").arg(i), script_model.getIcon(i));
		}
    }
    //Setting default tab position
    if(!config.contains("TabPosition"))
    {
        tab_position = West;
        config.setValue("TabPosition", tab_position);
    }
    //Setting default icons size
    if(!config.contains("IconsSize")) {
        icons_size = 16;
        config.setValue("IconsSize", icons_size);
    }
    //Setting default icons size
    if(!config.contains("IconsUseSystemTheme")) {
#if defined Q_WS_X11 /* Qt4 */ || defined Q_OS_LINUX /* Qt5 */
        icons_use_system_theme = true;
#else
        icons_use_system_theme = false;
#endif
        config.setValue("IconsUseSystemTheme", icons_use_system_theme);
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

    // Build two dictionaries of translation files for the program:
    // 1) the program translation files;
    // 2) qt library translation files.
    loadLanguages();
	//
#ifdef unix
	//Fixing Qt's problem on unix systems...
	QString system_lang(qgetenv("LANG").constData());
	system_lang.truncate(system_lang.indexOf('.'));
	if(system_lang.size()) locale_system = QLocale(system_lang);
	else locale_system = QLocale::system().language();
#else
	locale_system = QLocale::system().language();
#endif
	locale = (language_custom)?locale_current:locale_system;

    // Define priority locale for user interface.
    // Analyze only the program translation files dictionary.
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

    // Load files from dictionary for defined locale.
    updateLocale();

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
#if defined Q_WS_MAC /* Qt4 */ || defined Q_OS_MAC /* Qt5 */
	translation_dirs << QCoreApplication::applicationDirPath()+"/../Resources";
#endif
#ifdef Q_OS_UNIX
	translation_dirs << QDir::homePath()+"/.local/share/znotes/translations";
#endif
    // next path for qt library translation files
    // For MS Windows such files generally place in the program directory.
    translation_dirs << QLibraryInfo::location(QLibraryInfo::TranslationsPath);

	//looking for qm-files in translation directories
    QMap<int, QMap<int, QString> > *currTranslations;
    QString dirSearchTemplate;
    for(int i = 1; i <= 2; ++i) {
        switch (i) {
        case 1:
            currTranslations = &translations;
            dirSearchTemplate = "znotes_*.qm";
            break;

        case 2:
            currTranslations = &qtTranslations;
            dirSearchTemplate = "qt_*.qm";
            break;

        default:
            ;
        }
        QStringListIterator dir_path(translation_dirs);
        while(dir_path.hasNext())
        {
            QDir dir(dir_path.next());
            QStringList fileNames = dir.entryList(QStringList(dirSearchTemplate));
            for(int i=0; i < fileNames.size(); ++i)
            {
                QString filename(fileNames[i]);
                QString fullpath(dir.absoluteFilePath(filename));
                filename.remove(0, filename.indexOf('_') + 1);
                filename.chop(3);
                QLocale locale(filename);
                if(!(*currTranslations)[locale.language()].contains(locale.country()))
                    (*currTranslations)[locale.language()][locale.country()] = fullpath;
            }
        }
        //Setting default(English) translation path if other translation not found
        if(!(*currTranslations)[QLocale::English].contains(QLocale::UnitedStates))
            (*currTranslations)[QLocale::English][QLocale::UnitedStates]="";
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
void Settings::setNoteHighlight(bool v)
{
	if(note_highlight != v)
	{
		note_highlight = v;
		config.setValue("NoteHighlight", note_highlight);
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
  Saving notes's option for pasting to HTML notes only plaintext
*/
void Settings::setNotePastePlaintext(bool v)
{
	if(note_paste_plaintext != v)
	{
		note_paste_plaintext = v;
		config.setValue("NotePastePlaintext", note_paste_plaintext);
		emit NotePastePlaintextChanged();
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
void Settings::setToolbarItems(const QVector<int>& v) {
    if (v == tb_items) return;

    // Remove old settings
    for (int i = 0; i < tb_items.size(); ++i) {
        if(tb_items[i] != itemSeparator) {
            auto action = ToolbarAction(item_enum(tb_items[i]));
            config.remove(action.pref_name()); //dirty hack =(
	tb_items = v;
        }
    }
    // Save settings
	config.setValue("Toolbar/itemCount", tb_items.size());
    for (int i = 0; i < tb_items.size(); ++i) {
        if(tb_items[i] != itemSeparator) {
            auto action = ToolbarAction(item_enum(tb_items[i]));
            config.setValue(action.pref_name(), i);
        }
    }

    emit ToolbarItemsChanged();
}

/*
  Saving highlight rules
*/

void Settings::setHighlightRules(const QVector<HighlightRule>& v)
{
	if(highlight_rules==v) return;
	highlight_rules = v;
	config.beginWriteArray("HighlightRules", highlight_rules.size());
	for(int i=0; i<highlight_rules.size(); ++i)
	{
		config.setArrayIndex(i);
		config.setValue("enabled", highlight_rules.at(i).enabled);
		config.setValue("regexp", highlight_rules.at(i).regexp);
		config.setValue("color", highlight_rules.at(i).color.name());
	}
	config.endArray();
	emit NoteHighlightChanged();
}

/*
  Saving current language
*/
void Settings::setLocaleCurrent(const QLocale& new_locale)
{
	if(locale_current != new_locale)
	{
		locale_current = new_locale;
		config.setValue("LanguageCurrent", locale_current.name());
	}
	if(language_custom)
	{
		locale = locale_current;
		updateLocale();
	}
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
		if(!language_custom)
		{
			locale = locale_system;
			updateLocale();
		}
	}
}


void Settings::setIconsUseSystemTheme(bool v) {
    if(icons_use_system_theme == v) return;

    icons_use_system_theme = v;
    config.setValue("IconsUseSystemTheme", icons_use_system_theme);
    // TODO: doesn't help
    emit ToolbarItemsChanged();
}

void Settings::setIconsSize(int v) {
    if (icons_size == v) return;

    icons_size = v;
    config.setValue("IconsSize", icons_size);
    emit IconsSizeChanged();
}

/*
  Setting language
*/
void Settings::updateLocale()
{
    // Load a translation file for the program.
    translator.load(translations[locale.language()][locale.country()]);
    // Load a translation file for Qt library.
    qtranslator.load(qtTranslations[locale.language()][locale.country()]);
}
