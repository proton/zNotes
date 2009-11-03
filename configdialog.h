#ifndef CONFIGDIALOG_H
#define CONFIGDIALOG_H

#include <QtGui/QDialog>
#include <QAbstractButton>

namespace Ui {
	class configDialog;
}

class configDialog : public QDialog {
	Q_OBJECT
public:
	configDialog(QWidget *parent = 0);
	~configDialog();

private:
	Ui::configDialog *m_ui;
	void SaveSettings();

private slots:
	void on_btn_ScriptAdd_clicked();
	void on_btn_ScriptRemove_clicked();
	void on_btn_FontChange_clicked();
	void on_btn_NotesPath_clicked();
	void on_buttonBox_clicked(QAbstractButton* button);
};

#endif // CONFIGDIALOG_H
