#ifndef NOTE_H
#define NOTE_H

#include <QTextEdit>
#include <QFile>
#include <QDir>
#include <QSyntaxHighlighter>

class Highlighter : public QSyntaxHighlighter
{
	//Q_OBJECT
public:
	Highlighter(QTextDocument *parent = 0);
protected:
	void highlightBlock(const QString &text);
private:
	struct HighlightingRule
	{
		QRegExp pattern;
		QTextCharFormat format;
	};
	QVector<HighlightingRule> highlightingRules;
public:
	QTextCharFormat linkFormat;
};

class Note : public QTextEdit
{
	//Q_OBJECT
private:
	enum type
	{
		text, html
	};
public:
	Note(const QString& fn, const QDir&, const QFont&);
	QString name;
	QFile file;
	bool hasChange;
private:
	Highlighter *highlighter;
	void mousePressEvent(QMouseEvent *e);
	void mouseMoveEvent(QMouseEvent *e);
	inline bool isOnLink(const QTextCursor& cursor, int& pos_start, int& pos_end) const;
};

#endif // NOTE_H
