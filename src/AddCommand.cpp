#include "AddCommand.h"

AddCommand::AddCommand(MainWindow* mainwindow, QUndoCommand* parent) :
	mainwindow(mainwindow),
	QUndoCommand(parent)
{
	this->item = NULL;
	this->message = "";
}

AddCommand::~AddCommand()
{

}

void AddCommand::undo()
{
	if (this->mainwindow->CheckIfItemIsPresentInKeyList(this->item) == true)
	{
		this->mainwindow->RemoveKey(this->item);
	}

	this->mainwindow->PrintStatusMessage("Undo '" + this->message + "'");
}

void AddCommand::redo()
{
	if (this->mainwindow->CheckIfItemIsPresentInKeyList(this->item) == false)
	{
		this->keyName = "New_Key";

		int num = 0;
        while (this->mainwindow->CheckIfItemIsPresentInKeyList(this->keyName) == true)
		{
			this->keyName = "New_Key_" + QString::number(num);
			num++;
		}

		this->message = "Add Key \"" + this->keyName + "\"";
		this->setText(this->message);

        this->item = this->mainwindow->AddNewKey(this->keyName, this->item == NULL);
	}

	this->mainwindow->PrintStatusMessage(this->message);
}
