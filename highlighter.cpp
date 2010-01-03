#include "highlighter.h"
#include "settings.h"

Highlighter::Highlighter(QTextDocument *parent)
	: QSyntaxHighlighter(parent)
{
	HighlightingRule rule;

	linkFormat.setForeground(Qt::blue);
	rule.format = linkFormat;
	rule.pattern = QRegExp("http://\\S+");
	highlightingRules.append(rule);
}

void Highlighter::highlightBlock(const QString &text)
{
	if(settings.getNoteLinksHighlight())
	{
		foreach (const HighlightingRule &rule, highlightingRules)
		{
			QRegExp expression(rule.pattern);
			int index = expression.indexIn(text);
			while (index >= 0)
			{
				int length = expression.matchedLength();
				setFormat(index, length, rule.format);
				index = expression.indexIn(text, index + length);
			}
		}
	}
	setCurrentBlockState(0);
}
