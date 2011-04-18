#include "highlighter.h"
#include "settings.h"

Highlighter::Highlighter(QTextDocument *parent)
	: QSyntaxHighlighter(parent)
{
}

void Highlighter::highlightBlock(const QString &text)
{
	if(settings.getNoteHighlight())
	{
		foreach (const HighlightRule &rule, settings.getHighlightRules())
		{
			if(!rule.enabled) continue;
			if(rule.regexp.isEmpty()) continue;
			QRegExp expression(rule.regexp);
			if(!expression.isValid()) continue;
			QTextCharFormat format;
			format.setForeground(rule.color);
			int index = expression.indexIn(text);
			while (index >= 0)
			{
				int length = expression.matchedLength();
				setFormat(index, length, format);
				index = expression.indexIn(text, index+length);
			}
		}
	}
	setCurrentBlockState(0);
}
