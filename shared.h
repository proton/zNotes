#include <QVector>
#include <QShortcut>

#ifndef SHARED_H
#define SHARED_H

class Shared
{
public:
    Shared();
    static const QVector<QShortcut*>& shortcuts();
    static void addShortcut(QShortcut* shortcut, QString message = QString());
};

#endif // SHARED_H
