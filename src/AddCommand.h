#ifndef ADDCOMMAND_H
#define ADDCOMMAND_H

#include <QString>
#include <QUndoCommand>

#include "MainWindow.h"

class AddCommand : public QUndoCommand
{
public:
	AddCommand(MainWindow* mainwindow, QUndoCommand* parent = 0);
	virtual ~AddCommand();

	void undo() override;
	void redo() override;

private:
	MainWindow* mainwindow;

	QListWidgetItem* item;

	QString message;
	QString keyName;
};

#endif // ADDCOMMAND_H
