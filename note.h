#ifndef NOTE_H
#define NOTE_H

#include <QPlainTextEdit>
#include <QFile>

class Note : public QPlainTextEdit
{
	public:
		Note(const QString& fn, const QDir&);
		QString name;
		QFile file;
		bool hasChange;
};

#endif // NOTE_H
