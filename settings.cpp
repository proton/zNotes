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
		NotesPath = config.value("NotesPath").toString();
		LastNote = config.value("LastNote").toString();
		HideStart = config.value("HideStart").toBool();
		//
		DialogGeometry = config.value("DialogGeometry").toByteArray();
		DialogState = config.value("DialogState").toByteArray();
		//
		HideFrame = config.value("HideFrame").toBool();
		StayTop = config.value("StayTop").toBool();
		//
		NoteFont.fromString(config.value("NoteFont").toString());
		NoteLinksHighlight = config.value("NoteLinksHighlight").toBool();
		NoteLinksOpen = config.value("NoteLinksOpen").toBool();
		//
		int ScriptCount = config.value("ComandCount").toInt();
		for(int i=0; i<ScriptCount; ++i)
		{
			script_model.append(
				config.value(QString("ComandName%1").arg(i)).toString(),
				config.value(QString("ComandFile%1").arg(i)).toString(),
				config.value(QString("ComandIcon%1").arg(i)).toString());
		}
		ScriptShowOutput = config.value("ScriptShowOutput").toBool();
		ScriptCopyOutput = config.value("ScriptCopyOutput").toBool();
		//
		LanguageCustom = config.value("LanguageCustom").toBool();
		LanguageCurrent = QLocale(config.value("LanguageCurrent").toString()).language();
		//
		///Deprecated:
		///TODO: remove this code, when in version 0.4.1:
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
		else
		{
			bool old_settings_exist = (
					config.contains("tbHideEdit") ||
					config.contains("tbHideMove") ||
					config.contains("tbHideCopy") ||
					config.contains("tbHideSetup") ||
					config.contains("tbHideRun") ||
					config.contains("tbHideExit") );
			if(old_settings_exist) //converting old toolbar's settings
			{
				if(!config.value("tbHideEdit").toBool())
				{
					tb_items.append(itemAdd);
					tb_items.append(itemRemove);
					tb_items.append(itemRename);
					tb_items.append(itemSeparator);
				}
				if(!config.value("tbHideMove").toBool())
				{
					tb_items.append(itemPrev);
					tb_items.append(itemNext);
					tb_items.append(itemSeparator);
				}
				if(!config.value("tbHideCopy").toBool())
				{
					tb_items.append(itemCopy);
					tb_items.append(itemSeparator);
				}
				if(!config.value("tbHideSetup").toBool())
				{
					tb_items.append(itemSetup);
					tb_items.append(itemInfo);
					tb_items.append(itemSeparator);
				}
				if(!config.value("tbHideRun").toBool())
				{
					tb_items.append(itemRun);
					tb_items.append(itemSearch);
					tb_items.append(itemSeparator);
				}
				if(!config.value("tbHideExit").toBool()) tb_items.append(itemExit);
			}
		}
	}//TODO:
	else //if settings don't exist - setup default settings
	{
		//Setting default toolbar items
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
		//Setting default path to notes
	#ifdef Q_WS_X11
		NotesPath = QDir::homePath()+"/.local/share/notes";
		config.setValue("NotesPath", NotesPath);
	#endif
		//Setting default note options
		NoteLinksHighlight = true;
		config.setValue("NoteLinksHighlight", NoteLinksHighlight);
		NoteLinksOpen = true;
		config.setValue("NoteLinksOpen", NoteLinksOpen);
		//Setting default scripts
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
	loadLanguages();
	//
#ifdef unix
	//Fixing Qt's problem on unix systems...
	QString system_lang(qgetenv("LANG").constData());
	system_lang.truncate(system_lang.indexOf('.'));
	if(system_lang.size()>0) system_language = QLocale(system_lang).language();
	else system_language = QLocale::system().language();
#else
	system_language = QLocale::system().language();
#endif
	QLocale::Language lang = (LanguageCustom)?LanguageCurrent:system_language;
	if(!translations.contains(lang)) lang = QLocale::English;
	qtranslator.load("qt_"+QLocale(lang).name(), QLibraryInfo::location(QLibraryInfo::TranslationsPath));
	qApp->installTranslator(&qtranslator);
	setLanguage(lang);
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
			if(!translations.contains(locale.language()))
			{
				translations[locale.language()]=fullpath;
			}
		}
	}
	translations[QLocale::English]="";
}

/*
  Saving notes's path
*/
void Settings::setNotesPath(const QString& path)
{
	if(NotesPath != path)
	{
		NotesPath = path;
		config.setValue("NotesPath", NotesPath);
		emit NotesPathChanged();
	}
}

/*
  Saving title of last note
*/
void Settings::setLastNote(const QString& note_name)
{
	if(LastNote != note_name)
	{
		LastNote = note_name;
		config.setValue("LastNote", LastNote);
	}
}

/*
  Saving option (hiding dialog on start)
*/
void Settings::setHideStart(bool hide)
{
	if(HideStart != hide)
	{
		HideStart = hide;
		config.setValue("HideStart", HideStart);
	}
}

/*
  Saving option (hiding window decoration)
*/
void Settings::setHideFrame(bool Hide)
{
	if(HideFrame != Hide)
	{
		HideFrame = Hide;
		config.setValue("HideFrame", HideFrame);
		emit WindowStateChanged();
	}
}

/*
  Saving option (staying on top)
*/
void Settings::setStayTop(bool top)
{
	if(StayTop != top)
	{
		StayTop = top;
		config.setValue("StayTop", StayTop);
		emit WindowStateChanged();
	}
}

/*
  Saving dialog's params
*/
void Settings::setDialogGeometry(const QByteArray& g)
{
	DialogGeometry = g;
	config.setValue("DialogGeometry", DialogGeometry);
}
void Settings::setDialogState(const QByteArray& g)
{
	DialogState = g;
	config.setValue("DialogState", DialogState);
}

/*
  Saving notes's font
*/
void Settings::setNoteFont(const QFont& f)
{
	if(NoteFont != f)
	{
		NoteFont = f;
		config.setValue("NoteFont", NoteFont.toString());
		emit NoteFontChanged();
	}
}

/*
  Saving notes's links highlight option
*/
void Settings::setNoteLinksHighlight(bool b)
{
	if(NoteLinksHighlight != b)
	{
		NoteLinksHighlight = b;
		config.setValue("NoteLinksHighlight", NoteLinksHighlight);
		emit NoteHighlightChanged();
	}
}

/*
  Saving notes's links highlight option
*/
void Settings::setNoteLinksOpen(bool b)
{
	if(NoteLinksOpen != b)
	{
		NoteLinksOpen = b;
		config.setValue("NoteLinksOpen", NoteLinksOpen);
		emit NoteLinkOpenChanged();
	}
}

/*
  Saving script's options
*/
void Settings::setScriptShowOutput(bool sso)
{
	if(ScriptShowOutput != sso)
	{
		ScriptShowOutput = sso;
		config.setValue("ScriptShowOutput", ScriptShowOutput);
	}
}
void Settings::setScriptCopyOutput(bool sco)
{
	if(ScriptCopyOutput != sco)
	{
		ScriptCopyOutput = sco;
		config.setValue("ScriptCopyOutput", ScriptCopyOutput);
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
  Saving custom language
*/
void Settings::setLanguageCurrent(QLocale::Language l)
{
	if(LanguageCurrent != l)
	{
		LanguageCurrent = l;
		config.setValue("LanguageCurrent", QLocale(LanguageCurrent).name());
	}
	if(LanguageCustom) setLanguage(LanguageCurrent);
}

/*
  Saving option of using cusrom language
*/
void Settings::setLanguageCustom(bool b)
{
	if(LanguageCustom != b)
	{
		LanguageCustom = b;
		config.setValue("LanguageCustom", LanguageCustom);
		if(!LanguageCustom) setLanguage(QLocale(system_language).language());
	}
}

/*
  Setting language
*/
void Settings::setLanguage(QLocale::Language language)
{
	qtranslator.load("qt_"+QLocale(language).name(), QLibraryInfo::location(QLibraryInfo::TranslationsPath));
	translator.load(translations[language]);
}
