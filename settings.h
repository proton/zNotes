#ifndef SETTINGS_H
#define SETTINGS_H

#include <QString>

namespace Settings
{
	void Load();
	//
	const QString& getNotesPath();
	bool getHideStart();
	//
	void setNotesPath(const QString& path);
	void setHideStart(bool hide);
}

#endif // SETTINGS_H
