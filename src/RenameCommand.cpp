#include "RenameCommand.h"

RenameCommand::RenameCommand(MainWindow* mainwindow, QListWidgetItem *item, const QString& oldKey, const QString& newKey, QUndoCommand* parent) :
	mainwindow(mainwindow),
	item(item),
	oldKey(oldKey),
	newKey(newKey),
	QUndoCommand(parent)
{
	this->message = "";
}

RenameCommand::~RenameCommand()
{

}

void RenameCommand::undo()
{
	if (this->mainwindow->CheckIfItemIsPresentInKeyList(this->item) == false)
	{
		this->item = this->mainwindow->GetItemInKeyList(this->newKey);
	}

	if (this->item != NULL)
	{
		this->mainwindow->ApplyRenameKey(this->item, this->oldKey);
	}

	this->mainwindow->PrintStatusMessage("Undo '" + this->message + "'");
}

void RenameCommand::redo()
{
	if (this->mainwindow->CheckIfItemIsPresentInKeyList(this->item) == false)
	{
		this->item = this->mainwindow->GetItemInKeyList(this->oldKey);
	}

	if (this->item != NULL)
	{
		this->message = "Rename Key \"" + this->oldKey + "\" -> \"" + this->newKey + "\"";
		this->setText(this->message);

		this->mainwindow->ApplyRenameKey(this->item, this->newKey);
	}

	this->mainwindow->PrintStatusMessage(this->message);
}
