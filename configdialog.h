#ifndef CONFIGDIALOG_H
#define CONFIGDIALOG_H

#include <QDialog>
#include <QAbstractButton>
#include <QModelIndex>

class ToolbarItems;
class ItemModel;
class ItemToolbarModel;
class HighlightRuleModel;

namespace Ui {
	class configDialog;
}

class configDialog : public QDialog {
	Q_OBJECT
public:
	configDialog(QWidget *parent = 0);
	~configDialog();
	//
	void changeTabToFirst();
	void changeTabToCommands();

private:
	Ui::configDialog *m_ui;
	void SaveSettings();
	void changeEvent(QEvent *e);

	ToolbarItems* t_items;
	ItemModel* m_items;
	ItemToolbarModel* mt_items;
	HighlightRuleModel* highlight_rule_model;

private slots:
	void currentToolbarActionChanged(QModelIndex, QModelIndex);
	void currentListActionChanged(QModelIndex, QModelIndex);
	void currentHighlightRuleModelRowChanged(QModelIndex, QModelIndex index = QModelIndex());
	//
	void on_butActionBottom_clicked();
	void on_butActionTop_clicked();
	void on_butActionRemove_clicked();
	void on_butActionAdd_clicked();
	void on_btn_ScriptAdd_clicked();
	void on_btn_ScriptRemove_clicked();
	void on_btn_FontChange_clicked();
	void on_btn_NotesPath_clicked();
	void on_buttonBox_clicked(QAbstractButton* button);
	void on_highlightRuleAddButton_clicked();
	void on_highlightRuleRemoveButton_clicked();
	void on_highlightRuleUpButton_clicked();
	void on_highlightRuleDownButton_clicked();
};

#endif // CONFIGDIALOG_H
