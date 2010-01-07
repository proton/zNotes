#include "aboutDialog.h"
#include "ui_aboutDialog.h"

#ifndef VERSION
#define VERSION "unknown"
#endif

aboutDialog::aboutDialog(QWidget *parent) :
	QDialog(parent), ui(new Ui::aboutDialog)
{
	ui->setupUi(this);
	ui->lbAbout->setText(QString("zNotes\n%1\n%2 %3\n%4\n%5\n\n%6")
		.arg(tr("Simple Qt-based notes aplication"))
		.arg(tr("version")).arg(VERSION)
		.arg(tr("Copyright (c) 2010"))
		.arg(tr("by Peter Savichev (proton)"))
		.arg(tr("Distributed under the GPL license version 3 or later")));
	//
	QString strAuthors;
	strAuthors.append(QString("<p><b>%1</b> - <a href='mailto:%2'>%2</a><br/>%3<p>")
		.arg("Peter Savichev (proton)").arg("psavichev@gmail.com")
		.arg(tr("Application concept and main development")));
	strAuthors.append(QString("<p><b>%1</b></p>").arg(tr("Translations:")));
	strAuthors.append(QString("<p><b>%1</b> - <a href='mailto:%2'>%2</a><br/>%3</p>")
		.arg("Peter Savichev (proton)").arg("psavichev@gmail.com")
		.arg(tr("Russian translation")));
	strAuthors.append(QString("<p><b>%1</b></p>").arg(tr("Icons:")));
	strAuthors.append(QString("%1\n<a hreh='%2'>%2</a>")
		.arg(tr("All icons are modified icons from Oxygen project")).arg("http://www.oxygen-icons.org"));
	ui->lbAuthors->setHtml(strAuthors);
	//
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
		.arg("Oktay Cetinkaya (xelat)").arg("xelat09@yahoo.de")
		.arg("Windows"));
	strCredits.append(QString("<p><b>%1</b> - <a href='mailto:%2'>%2</a><br/>%3</p>")
		.arg("Elbert Pol").arg("e.pol@chello.nl")
		.arg("OS/2"));
	ui->lbCredits->setHtml(strCredits);
	QString strHelp;
	strHelp.append(QString("<p><b>%1</b><p>").arg(tr("Shortcuts:")));
	strHelp.append(QString("<b>%1</b> - %2<br/>").arg("F2").arg("Rename current note"));
	ui->lbHelp->setHtml(strHelp);
	adjustSize();
}

aboutDialog::~aboutDialog()
{
	delete ui;
}
