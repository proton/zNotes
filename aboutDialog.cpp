#include "aboutDialog.h"
#include "ui_aboutDialog.h"

//Fixed problems with compilation, if program version is undefined
#ifndef VERSION
#define VERSION "unknown"
#endif

aboutDialog::aboutDialog(QWidget *parent) :
	QDialog(parent), ui(new Ui::aboutDialog)
{
	ui->setupUi(this);
	//Adding program information
	ui->lbAbout->setText(QString("zNotes\n%1\n%2 %3\n%4\n%5\n\n%6")
		.arg(tr("Simple Qt-based notes aplication"))
		.arg(tr("version")).arg(VERSION)
		.arg(tr("Copyright (c) 2010"))
		.arg(tr("by Peter Savichev (proton)"))
		.arg(tr("Distributed under the GPL license version 3 or later")));
	//Adding authors information
	QString strAuthors;
	strAuthors.append(QString("<p><b>%1</b> - <a href='mailto:%2'>%2</a><br/>%3<p>")
		.arg("Peter Savichev (proton)").arg("psavichev@gmail.com")
		.arg(tr("Application concept and main development")));
	strAuthors.append(QString("<p><b>%1</b></p>").arg(tr("Translations:")));
	strAuthors.append(QString("<p><b>%1</b> - <a href='mailto:%2'>%2</a><br/>%3</p>")
		.arg("Pavel Fric (fri)").arg("pavelfric@seznam.cz")
		.arg(tr("Czech translation")));
	strAuthors.append(QString("<p><b>%1</b> - <a href='mailto:%2'>%2</a><br/>%3</p>")
		.arg("Peter Savichev (proton)").arg("psavichev@gmail.com")
		.arg(tr("Russian translation")));
	strAuthors.append(QString("<p><b>%1</b></p>").arg(tr("Icons:")));
	strAuthors.append(QString("%1\n<a hreh='%2'>%2</a>")
		.arg(tr("All icons are modified icons from Oxygen project")).arg("http://www.oxygen-icons.org"));
	ui->lbAuthors->setHtml(strAuthors);
	//Adding credits information
	QString strCredits;
	strCredits.append(QString("<p><b>%1</b><p>").arg(tr("Testing:")));
	strCredits.append(QString("<b>%1</b> - <a href='mailto:%2'>%2</a><br/>").arg("Nikolay Edigaryev").arg("edigaryev@gmail.com"));
	strCredits.append(QString("<b>%1</b> - <a href='mailto:%2'>%2</a><br/>").arg("Nikita Lyalin (tinman321)").arg("tinman321@gmail.com"));
	strCredits.append(QString("<b>%1</b> - <a href='mailto:%2'>%2</a><br/>").arg("Elbert Pol").arg("e.pol@chello.nl"));
	strCredits.append(QString("<b>%1</b> - <a href='mailto:%2'>%2</a><br/>").arg("Andrew Bodrow (Drino)").arg("drinob@gmail.com"));
	strCredits.append(QString("<b>%1</b> - <a href='mailto:%2'>%2</a><br/>").arg("Max Porozkov (MaxPv)").arg("maxpv@rusnix.ru"));
	strCredits.append(QString("<p><b>%1</b></p>").arg(tr("Releasing:")));
	strCredits.append(QString("<p><b>%1</b> - <a href='mailto:%2'>%2</a><br/><b>%3</b> - <a href='mailto:%4'>%4</a><br/>%5</p>")
		.arg("Peter Savichev (proton)").arg("psavichev@gmail.com")
		.arg("Ben de Groot (yngwin)").arg("yngwin@gentoo.org ")
		.arg("Gentoo"));
	strCredits.append(QString("<p><b>%1</b> - <a href='mailto:%2'>%2</a><br/>%3</p>")
		.arg("Nikita Lyalin (tinman321)").arg("tinman321@gmail.com")
		.arg("ArchLinux"));
	strCredits.append(QString("<p><b>%1</b> - <a href='mailto:%2'>%2</a><br/>%3</p>")
		.arg("Eugene Pivnev (ti.eugene)").arg("ti.eugene@gmail.com")
		.arg("Fedora, Mandriva, OpenSuse, Ubuntu"));
	strCredits.append(QString("<p><b>%1</b> - <a href='mailto:%2'>%2</a><br/>%3</p>")
		.arg("Veniamin Gvozdikov (zloidemon)").arg("g.veniamin@googlemail.com")
		.arg("FreeBSD"));
	strCredits.append(QString("<p><b>%1</b> - <a href='mailto:%2'>%2</a><br/>%3</p>")
		.arg("Oktay Cetinkaya (xelat)").arg("xelat09@yahoo.de")
		.arg("Windows"));
	strCredits.append(QString("<p><b>%1</b> - <a href='mailto:%2'>%2</a><br/>%3</p>")
		.arg("Elbert Pol").arg("e.pol@chello.nl")
		.arg("OS/2"));
	ui->lbCredits->setHtml(strCredits);
	//Adding help information
	QString strHelp;
	strHelp.append(QString("<p><b>%1</b><p>").arg(tr("Shortcuts:")));
	strHelp.append(QString("<b>%1</b> - %2<br/>").arg("Alt+Fn").arg(tr("Go to note n")));
	strHelp.append(QString("<b>%1</b> - %2<br/>").arg("Alt+Left").arg(tr("Go to previous note")));
	strHelp.append(QString("<b>%1</b> - %2<br/>").arg("Alt+Right").arg(tr("Go to next note")));
	strHelp.append(QString("<b>%1</b> - %2<br/>").arg("Insert").arg(tr("Create new note")));
	strHelp.append(QString("<b>%1</b> - %2<br/>").arg("Delete").arg(tr("Remove current note")));
	strHelp.append(QString("<b>%1</b> - %2<br/>").arg("F2").arg(tr("Rename current note")));
	strHelp.append(QString("<b>%1</b> - %2<br/>").arg("Ctrl+F").arg(tr("Search in the notes' text")));
	strHelp.append(QString("<b>%1</b> - %2<br/>").arg("Ctrl+B").arg(tr("Make selected text bold")));
	strHelp.append(QString("<b>%1</b> - %2<br/>").arg("Ctrl+I").arg(tr("Make selected text italic")));
	strHelp.append(QString("<b>%1</b> - %2<br/>").arg("Ctrl+S").arg(tr("Make selected text strikeout")));
	strHelp.append(QString("<b>%1</b> - %2<br/>").arg("Ctrl+U").arg(tr("Make selected text underline")));
	strHelp.append(QString("<b>%1</b> - %2<br/>").arg("Ctrl+Q").arg(tr("Exit program")));
	ui->lbHelp->setHtml(strHelp);
	//adjustSize();
}

aboutDialog::~aboutDialog()
{
	delete ui;
}
