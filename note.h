#ifndef NOTE_H
#define NOTE_H

#include <QPlainTextEdit>
#include <QFile>

class Note : public QPlainTextEdit
{
	enum type
	{
		text, html
	};
	public:
		Note(const QString& fn, const QDir&, const QFont&);
		QString name;
		QFile file;
		bool hasChange;
};

#endif // NOTE_H
