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
		.arg(tr("Copyright (c) 2013"))
		.arg(tr("by Peter Savichev (proton)"))
		.arg(tr("Distributed under the GPL license version 3 or later")));
	//Adding authors information
	QString strAuthors;
	strAuthors.append(QString("<p><b>%1</b> - <a href='mailto:%2'>%2</a><br/>%3<p>")
		.arg("Peter Savichev (proton)", "psavichev@gmail.com", tr("Application concept and main development")));
	strAuthors.append(QString("<p><b>%1</b></p>").arg(tr("Translations:")));
	strAuthors.append(QString("<p><b>%1</b> - <a href='mailto:%2'>%2</a><br/>%3</p>")
		.arg("Marcio Moraes (marciopanto)", "marciopanto@gmail.com", tr("Brazilian Portuguese translation")));
	strAuthors.append(QString("<p><b>%1</b> - <a href='mailto:%2'>%2</a><br/>%3</p>")
		.arg("Pavel Fric (fri)", "pavelfric@seznam.cz", tr("Czech translation")));
	strAuthors.append(QString("<p><b>%1</b> - <a href='mailto:%2'>%2</a><br/>%3</p>")
		.arg("Jan Rimmek", "jan@rimmek.de", tr("German translation")));
	strAuthors.append(QString("<p><b>%1</b> - <a href='mailto:%2'>%2</a><br/>%3</p>")
		.arg("Peter Savichev (proton)", "psavichev@gmail.com", tr("Russian translation")));
	strAuthors.append(QString("<p><b>%1</b> - <a href='mailto:%2'>%2</a><br/>%3</p>")
		.arg("Slavko Fedorik", "slavino@slavino.sk", tr("Slovak translation")));
	strAuthors.append(QString("<p><b>%1</b> - <a href='mailto:%2'>%2</a><br/>%3</p>")
		.arg("Alieola", "aliaeolova@gmail.com", tr("Spanish translation")));
	strAuthors.append(QString("<p><b>%1</b> - <a href='mailto:%2'>%2</a><br/>%3</p>")
		.arg("lukethenuke", "", tr("Swedish translation")));
	strAuthors.append(QString("<p><b>%1</b> - <a href='mailto:%2'>%2</a><br/>%3</p>")
		.arg("Vadim Nekhai (onix)", "onix@onix.name", tr("Ukrainian translation")));
	strAuthors.append(QString("<p><b>%1</b> - <a href='mailto:%2'>%2</a><br/>%3</p>")
		.arg("Janusz J. Maczka (zzzzzzzzz@github)", "", tr("Polish translation")));
	strAuthors.append(QString("<p><b>%1</b></p>").arg(tr("Icons:")));
	strAuthors.append(QString("%1\n<a href ='%2'>%2</a>")
		.arg(tr("All icons are modified icons from Oxygen project"), "http://www.oxygen-icons.org"));
	ui->lbAuthors->setHtml(strAuthors);
	//Adding credits information
	QString strCredits;
	strCredits.append(QString("<p><b>%1</b><p>").arg(tr("Testing:")));
	strCredits.append(QString("<b>%1</b> - <a href='mailto:%2'>%2</a><br/>").arg("Nikolay Edigaryev", "edigaryev@gmail.com"));
	strCredits.append(QString("<b>%1</b> - <a href='mailto:%2'>%2</a><br/>").arg("Nikita Lyalin (tinman321)", "tinman321@gmail.com"));
	strCredits.append(QString("<b>%1</b> - <a href='mailto:%2'>%2</a><br/>").arg("Elbert Pol", "e.pol@chello.nl"));
	strCredits.append(QString("<b>%1</b> - <a href='mailto:%2'>%2</a><br/>").arg("Andrew Bodrow (Drino)", "drinob@gmail.com"));
	strCredits.append(QString("<b>%1</b> - <a href='mailto:%2'>%2</a><br/>").arg("Max Porozkov (MaxPv)", "maxpv@rusnix.ru"));
	strCredits.append(QString("<p><b>%1</b></p>").arg(tr("Releasing:")));
	strCredits.append(QString("<p><b>%1</b> - <a href='mailto:%2'>%2</a><br/><b>%3</b> - <a href='mailto:%4'>%4</a><br/>%5</p>")
		.arg("Peter Savichev (proton)", "psavichev@gmail.com", "Ben de Groot (yngwin)", "yngwin@gentoo.org ", "Gentoo"));
	strCredits.append(QString("<p><b>%1</b> - <a href='mailto:%2'>%2</a><br/>%3</p>")
		.arg("Nikita Lyalin (tinman321)", "tinman321@gmail.com", "ArchLinux"));
	strCredits.append(QString("<p><b>%1</b> - <a href='mailto:%2'>%2</a><br/>%3</p>")
		.arg("Eugene Pivnev (ti.eugene)", "ti.eugene@gmail.com", "Fedora, Mandriva, OpenSuse, Ubuntu"));
	strCredits.append(QString("<p><b>%1</b> - <a href='mailto:%2'>%2</a><br/>%3</p>")
		.arg("Veniamin Gvozdikov (zloidemon)", "g.veniamin@googlemail.com", "FreeBSD"));
	strCredits.append(QString("<p><b>%1</b> - <a href='mailto:%2'>%2</a><br/>%3</p>")
		.arg("Oktay Cetinkaya (xelat)", "xelat09@yahoo.de", "Windows"));
	strCredits.append(QString("<p><b>%1</b> - <a href='mailto:%2'>%2</a><br/>%3</p>")
		.arg("Elbert Pol", "e.pol@chello.nl", "OS/2"));
	ui->lbCredits->setHtml(strCredits);
	//Adding help information
	QString strHelp;
	strHelp.append(QString("<p><b>%1</b><p>").arg(tr("Shortcuts:")));
	strHelp.append(QString("<b>%1</b> - %2<br/>").arg("Alt+Fn").arg(tr("Go to note n")));
	strHelp.append(QString("<b>%1</b> - %2<br/>").arg("Alt+Left").arg(tr("Go to previous note")));
	strHelp.append(QString("<b>%1</b> - %2<br/>").arg("Alt+Right").arg(tr("Go to next note")));
	strHelp.append(QString("<b>%1</b> - %2<br/>").arg("Ctrl+N").arg(tr("Create new note")));
	strHelp.append(QString("<b>%1</b> - %2<br/>").arg("Ctrl+W").arg(tr("Remove current note")));
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
