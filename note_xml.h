#ifndef NOTE_XML_H
#define NOTE_XML_H

#include "note.h"

class QTreeWidget;
class QDomDocument;
class QDomElement;
class QTreeWidgetItem;

class XmlNote : public Note
{
	Q_OBJECT
public:
	XmlNote(const QFileInfo& fileinfo, Note::Type type_new);
	~XmlNote();
	void load(); //Reading note's content
	void save(bool forced = false); //Saving note's content
	void copy() const; //Coping note's content to clipboard
	QWidget* widget();
private:
	QTreeWidget* tree_widget;
	QDomDocument* document;
	//
	void apendTask(QDomElement* element, QTreeWidgetItem* parent = 0);
};

#endif // NOTE_XML_H
