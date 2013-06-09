#include "shared.h"

QVector<QShortcut*> p_shortcuts;

Shared::Shared()
{
}

const QVector<QShortcut*>& Shared::shortcuts()
{
    return p_shortcuts;
}

void Shared::addShortcut(QShortcut* shortcut, QString comment)
{
    shortcut->setWhatsThis(comment);
    p_shortcuts << shortcut;
}
