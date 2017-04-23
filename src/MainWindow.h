#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "ProjectHelper.h"

#include <QMainWindow>
#include <QTableWidgetItem>
#include <QListWidgetItem>
#include <QMap>

#include <QtWebEngine>
#include <QtWebEngineWidgets/QWebEngineView>

namespace Ui
{
class MainWindow;
}

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	explicit MainWindow(QWidget *parent = 0);
	virtual ~MainWindow();

	void LoadProject(const QString& path);

	void RefreshButtonStatus(int index);
	void RefreshListKeyStatus(QListWidgetItem* item);

private slots:
	void OnClickButtonStatus(int index);

	void on_searchKeyLineEdit_textChanged(const QString &arg1);

	void on_valueTableWidget_itemSelectionChanged();
	void on_valueTableWidget_itemChanged(QTableWidgetItem *item);

	void on_keyListWidget_itemSelectionChanged();
	void on_keyListWidget_itemChanged(QListWidgetItem *item);
	void on_keyListWidget_customContextMenuRequested(const QPoint &pos);
	void on_keyListWidget_customContextMenuTriggered(QAction* action);

	void on_actionNewProject_triggered();
	void on_actionOpenProject_triggered();
	void on_actionCloseProject_triggered();
	void on_actionSaveProject_triggered();
	void on_actionQuit_triggered();

	void on_actionProjectSettings_triggered();

	void on_addKeyButton_clicked();
	void on_renameKeyButton_clicked();
	void on_deleteKeyButton_clicked();

private:

	void CloseProject();

	void NewKey();
	void RenameKey();
	void DeleteKey();

	Ui::MainWindow *ui;

	QWebEngineView* webengine;

	QString dirPath;
	QString projectName;

	QString defaultLang;

	QListWidgetItem* selectedKeyItem;

	ProjectHelper project;
};

#endif // MAINWINDOW_H