#ifndef HIGHLIGHTER_H
#define HIGHLIGHTER_H

#include <QSyntaxHighlighter>

class Highlighter : public QSyntaxHighlighter
{
public:
	Highlighter(QTextDocument *parent = 0);
protected:
	void highlightBlock(const QString &text);
};

#endif // HIGHLIGHTER_H
