#ifndef RENAMECOMMAND_H
#define RENAMECOMMAND_H

#include <QString>
#include <QUndoCommand>

#include "MainWindow.h"

class RenameCommand : public QUndoCommand
{
public:
	RenameCommand(MainWindow* mainwindow, QListWidgetItem *item, const QString& oldKey, const QString& newKey, QUndoCommand* parent = 0);
	virtual ~RenameCommand();

	void undo() override;
	void redo() override;

private:
	MainWindow* mainwindow;

	QListWidgetItem* item;

	QString message;
	QString oldKey;
	QString newKey;
};

#endif // RENAMECOMMAND_H
