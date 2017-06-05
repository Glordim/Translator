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
	void on_projectLocationFindButton_clicked();

	void on_moveToSupportedButton_clicked();
	void on_moveToAvailableButton_clicked();

	void on_availableListWidget_itemDoubleClicked(QListWidgetItem *item);
	void on_supportedListWidget_itemDoubleClicked(QListWidgetItem *item);

private:

	void MoveToAvailable(QListWidgetItem *item);
	void MoveToSupported(QListWidgetItem *item);

	Ui::DialogCreateProject *ui;

	ProjectHelper& project;
};

#endif // DIALOGCREATEPROJECT_H
