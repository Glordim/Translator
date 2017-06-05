#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "ProjectHelper.h"

#include <QMap>
#include <QMainWindow>
#include <QListWidgetItem>
#include <QTableWidgetItem>

#ifdef USE_WEBENGINE
#include <QtWebEngine>
#include <QtWebEngineWidgets/QWebEngineView>
#endif

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

	bool LoadProject(const QString& path);

	void RefreshWebView();
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

#ifdef USE_WEBENGINE
	QWebEngineView* webengine;
#endif

	QString dirPath;
	QString projectName;

	QString defaultLang;

	QListWidgetItem* selectedKeyItem;

	ProjectHelper project;
};

#endif // MAINWINDOW_H
