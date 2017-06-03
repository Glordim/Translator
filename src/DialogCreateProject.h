#ifndef DIALOGCREATEPROJECT_H
#define DIALOGCREATEPROJECT_H

#include <QDialog>
#include <QAbstractButton>

#include <QListWidgetItem>

#include "ProjectHelper.h"

namespace Ui
{
class DialogCreateProject;
}

class DialogCreateProject : public QDialog
{
	Q_OBJECT

public:
	DialogCreateProject(QWidget *parent, ProjectHelper& project);
	virtual ~DialogCreateProject();

protected:
	void done(int r);

private slots:
	void on_toolButton_clicked();

	void on_ButtonAllToSupported_clicked();

	void on_ButtonSupportedToAll_clicked();

	void on_listWidget_itemDoubleClicked(QListWidgetItem *item);

	void on_listWidget_2_itemDoubleClicked(QListWidgetItem *item);

private:

	void MoveSupportedToAll(QListWidgetItem *item);
	void MoveAllToSupported(QListWidgetItem *item);

	Ui::DialogCreateProject *ui;

	ProjectHelper& project;
};

#endif // DIALOGCREATEPROJECT_H
