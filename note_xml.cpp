#include "note_xml.h"

#include <QClipboard>
#include <QApplication>

#include <QTreeWidget>
#include <QDomDocument>
#include <QDomElement>
#include <QHeaderView>
#include <QDateTime>

//document

//TODO: тут модель нужна таки...

XmlNote::XmlNote(const QFileInfo& fileinfo, Note::Type type_new)
	: Note(fileinfo, type_new)
{
	document = new QDomDocument();
	tree_widget = new QTreeWidget();
	tree_widget->header()->setResizeMode(QHeaderView::ResizeToContents);
	tree_widget->setColumnCount(3);
	//tree_widget->setHeaderHidden(true);
//	tree_widget->setHeaderLabel(title()+":");

	load(); //loading note's content
}

XmlNote::~XmlNote()
{
	delete tree_widget;
}

//Reading file
void XmlNote::load()
{
	file.close();
	if(!file.open(QIODevice::ReadOnly)) return;
	if(!document->setContent(&file))
	{
		file.close();
		return;
	}
	file.close();
	QDomElement docElem = document->documentElement();

	QDomNode n = docElem.firstChild();
	while(!n.isNull())
	{
		QDomElement e = n.toElement(); // try to convert the node to an element.
		if(!e.isNull() && e.tagName()=="task") apendTask(&e);
		n = n.nextSibling();
	}
}

void XmlNote::apendTask(QDomElement* element, QTreeWidgetItem* parent)
{
	QTreeWidgetItem* item_text = new QTreeWidgetItem(tree_widget);

	if(parent==0) tree_widget->insertTopLevelItem(0, item_text);
	else parent->insertChild(0, item_text);

	QDomNode n = element->firstChild();
	while(!n.isNull())
	{
		QDomElement e = n.toElement(); // try to convert the node to an element.
		if(!e.isNull())
		{
			if(e.tagName()=="title") item_text->setText(0, e.text());
			if(e.tagName()=="task") apendTask(&e);
		}
		n = n.nextSibling();
	}
}

//Saving note
void XmlNote::save(bool forced)
{
	if(!(content_changed || forced)) return; //If file doesn't need in saving, exiting from function
//	file.close();
//	if(!file.open(QFile::WriteOnly | QFile::Text)) return;
//	QTextStream out(&file);
//	out << document->toString();
//	file.close();
	content_changed = false;
}

//Returning widget (it's can be placed to tabwidget)
QWidget* XmlNote::widget()
{
	return tree_widget;
}

//Coping note's content to clipboard
void XmlNote::copy() const
{
	QClipboard* clipboard = QApplication::clipboard();
	clipboard->setText(document->toString());
}
