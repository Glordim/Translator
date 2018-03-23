#include "Singleton.h"
#include "LangHelper.h"
#include "MainWindow.h"
#include "ui_MainWindow.h"
#include "DialogCreateProject.h"

#include "AddCommand.h"
#include "RenameCommand.h"

#include "KeyListWidgetItem.h"

#include <iostream>
#include <QShortcut>
#include <QTextEdit>
#include <QScrollBar>
#include <QMessageBox>
#include <QFileDialog>
#include <QKeySequence>
#include <QSignalMapper>
#include <QErrorMessage>
#include <QStandardPaths>

MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow),
	project(this)
{
	this->ui->setupUi(this);

	this->undoStack = new QUndoStack(this);
	this->undoView = new QUndoView(this->undoStack);
	this->undoView->setWindowTitle(tr("Command List"));
	this->undoView->show();
	this->undoView->setAttribute(Qt::WA_QuitOnClose, false);

	this->undoAction = this->undoStack->createUndoAction(this, tr("&Undo"));
	this->undoAction->setShortcuts(QKeySequence::Undo);

	this->redoAction = this->undoStack->createRedoAction(this, tr("&Redo"));
	this->redoAction->setShortcuts(QKeySequence::Redo);

	QMenu* menuEdit = ui->menuBar->findChild<QMenu*>("menuEdit");

	menuEdit->addAction(undoAction);
	menuEdit->addAction(redoAction);

#ifdef USE_WEBENGINE
	this->webengine = new QWebEngineView(this->ui->valueAndWebEngineSplitter);
	this->webengine->show();
#endif

	QShortcut* shortcut = new QShortcut(QKeySequence::New, this->ui->keyListWidget);
	this->connect(shortcut, SIGNAL(activated()), this, SLOT(on_addKeyButton_clicked()));

	shortcut = new QShortcut(QKeySequence(Qt::Key_F2), this->ui->keyListWidget);
	this->connect(shortcut, SIGNAL(activated()), this, SLOT(on_renameKeyButton_clicked()));

	shortcut = new QShortcut(QKeySequence::Delete, this->ui->keyListWidget);
	this->connect(shortcut, SIGNAL(activated()), this, SLOT(on_deleteKeyButton_clicked()));

	this->CloseProject();
}

bool MainWindow::LoadProject(const QString& path)
{
	if (this->project.Load(path) == false)
		return false;

	this->defaultLang = this->project.GetDefaultLang();

	this->ui->keyListWidget->clear();

	QList<QString> langList = this->project.GetLangList();

	int langCount = langList.count();
	for (int i = 0; i < langCount; ++i)
	{
        QString lang = langList[i];
        const QMap<QString, KeyValue>& keyMap = this->project.LoadLang(lang);

        auto itKey = keyMap.keyBegin();
        auto itVal = keyMap.begin();

        while (itKey != keyMap.keyEnd())
        {
            QList<QListWidgetItem*> similarKey = this->ui->keyListWidget->findItems(*itKey, Qt::MatchExactly);

            KeyListWidgetItem* keyItem = nullptr;

            if (similarKey.size() == 0)
            {
                keyItem = new KeyListWidgetItem(*itKey, this->ui->keyListWidget);
            }
            else
            {
                keyItem = (KeyListWidgetItem*)similarKey[0];
            }

            keyItem->setData(Qt::UserRole, *itKey);
            keyItem->setFlags(keyItem->flags() | Qt::ItemIsEditable);

            keyItem->SetLang(langList);

            KeyValue& keyValue = keyItem->GetKeyValueByLang(lang);
            keyValue.SetValue(itVal->GetValue());
            keyValue.SetStatus(itVal->GetStatus());

            keyItem->RefreshGlobalStatus();

            ++itKey;
            ++itVal;
        }
	}

	QHeaderView* headers = new QHeaderView(Qt::Orientation::Horizontal);
	this->ui->valueTableWidget->setHorizontalHeader(headers);

	this->ui->valueTableWidget->setColumnCount(2);
	//this->ui->valueTableWidget->setHorizontalHeaderLabels({"Value", ""});
	this->ui->valueTableWidget->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
	this->ui->valueTableWidget->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Fixed);
	this->ui->valueTableWidget->setColumnWidth(1, 32);

	while (this->ui->valueTableWidget->rowCount() != 0)
	{
		this->ui->valueTableWidget->removeRow(this->ui->valueTableWidget->rowCount() -1);
	}

	QSignalMapper* signalMapper = new QSignalMapper(this);

	for (int i = 0; i < langCount; ++i)
	{
		this->ui->valueTableWidget->insertRow(this->ui->valueTableWidget->rowCount());

		QIcon icon;
		icon.addFile(":Ok");

		QPushButton* buttonStatus = new QPushButton();
		buttonStatus->setIcon(icon);
		buttonStatus->setIconSize(QSize(32,32));
		buttonStatus->setFlat(true);
		//buttonStatus->setStyleSheet("border: none;");

		signalMapper->setMapping(buttonStatus, i);
		connect(buttonStatus, SIGNAL(clicked(bool)), signalMapper, SLOT(map()));

		this->ui->valueTableWidget->setCellWidget(i, 1, buttonStatus);
	}

	connect(signalMapper, SIGNAL(mapped(int)), this, SLOT(OnClickButtonStatus(int)));

	//this->ui->valueTableWidget->horizontalHeader()->show();
	this->ui->valueTableWidget->horizontalHeader()->hide();
	this->ui->valueTableWidget->verticalHeader()->show();

	QList<QString> langDisplayNameList;
	LangHelper& langHelper = Singleton<LangHelper>::getInstance();

	for (int i = 0; i < langList.count(); ++i)
	{
		langDisplayNameList.push_back(langHelper.GetLangInfo(langList[i]).displayName);
	}

	this->ui->valueTableWidget->setVerticalHeaderLabels(langDisplayNameList);
	this->ui->valueTableWidget->verticalHeader()->setDefaultSectionSize(60);
	this->ui->valueTableWidget->verticalHeader()->resizeSections(QHeaderView::ResizeToContents);

	if (langList.count() != 0)
	{
		QTableWidgetItem* defaultLangRow = this->ui->valueTableWidget->verticalHeaderItem(langList.indexOf(this->defaultLang));
		QFont font = defaultLangRow->font();
		font.setBold(true);
		defaultLangRow->setFont(font);
	}

	this->ui->keyListWidget->setContextMenuPolicy(Qt::CustomContextMenu);

	if (this->ui->keyListWidget->count() != 0)
	{
        QListWidgetItem* item = this->ui->keyListWidget->item(0);
		item->setSelected(true);

		for (int i = 0; i < this->ui->keyListWidget->count(); ++i)
            ((KeyListWidgetItem*)this->ui->keyListWidget->item(i));
	}

	this->ui->keyListWidget->setEnabled(true);
	this->ui->valueTableWidget->setEnabled(true);
	this->ui->addKeyButton->setEnabled(true);
	this->ui->renameKeyButton->setEnabled(true);
	this->ui->deleteKeyButton->setEnabled(true);
	this->ui->searchKeyLineEdit->setEnabled(true);

	return true;
}

MainWindow::~MainWindow()
{
	delete ui;
}

ProjectHelper& MainWindow::GetProject()
{
	return this->project;
}

void MainWindow::OnClickButtonStatus(int index)
{
	if (this->selectedKeyItem == NULL)
		return;

	QString lang = this->project.GetLangList()[index];

    KeyStatus status = this->selectedKeyItem->GetKeyValueByLang(lang).GetStatus();

	if (status == KeyStatus::Block)
		return;

	if (status == KeyStatus::ToCheck)
		status = KeyStatus::Validate;
	else
		status = KeyStatus::ToCheck;

    this->selectedKeyItem->GetKeyValueByLang(lang).SetStatus(status);

	this->RefreshButtonStatus(index);

    this->selectedKeyItem->RefreshGlobalStatus();
}

void MainWindow::RefreshButtonStatus(int index)
{
	if (this->selectedKeyItem == NULL)
		return;

	QString lang = this->project.GetLangList()[index];
    KeyStatus status = this->selectedKeyItem->GetKeyValueByLang(lang).GetStatus();

	QPushButton* buttonStatus = (QPushButton*)this->ui->valueTableWidget->cellWidget(index, 1);

	if (buttonStatus == NULL)
		return;

	QIcon icon;
	icon.addFile(":" + KeyValue::StatusToString(status));

	buttonStatus->setIcon(icon);
}

//  === Search Key ===

void MainWindow::on_searchKeyLineEdit_textChanged(const QString& searchValue)
{
	int keyCount = this->ui->keyListWidget->count();
	for (int i = 0; i < keyCount; ++i)
	{
		bool finded = this->ui->keyListWidget->item(i)->text().contains(searchValue, Qt::CaseInsensitive);
		this->ui->keyListWidget->item(i)->setHidden(finded == false);
	}
}

//  === KeyList ===

void MainWindow::on_keyListWidget_itemSelectionChanged()
{
    QList<QListWidgetItem*> selectedKeys = this->ui->keyListWidget->selectedItems();

	this->ui->addKeyButton->setEnabled(selectedKeys.count() <= 1);
	this->ui->renameKeyButton->setEnabled(selectedKeys.count() == 1);
	this->ui->deleteKeyButton->setEnabled(selectedKeys.count() >= 1);

	if (selectedKeys.empty() == true || selectedKeys.count() > 1)
	{
        //this->ui->valueTableWidget->clearContents();
		this->ui->valueTableWidget->setEnabled(false);

		this->selectedKeyItem = NULL;

		return;
	}

	this->ui->valueTableWidget->setEnabled(true);

    this->selectedKeyItem = (KeyListWidgetItem*)selectedKeys[0];

	QList<QString> langList = this->project.GetLangList();

	int langCount = langList.count();
	for (int i = 0; i < langCount; ++i)
	{
        const QString& text = this->selectedKeyItem->GetKeyValueByLang(langList[i]).GetValue();
		QTableWidgetItem* item = this->ui->valueTableWidget->item(i, 0);

		if (item == NULL)
			this->ui->valueTableWidget->setItem(i, 0, new QTableWidgetItem(text));
		else
			item->setText(text);

        this->RefreshButtonStatus(i);
	}

	this->RefreshWebView();
}

//See closeEditor signal
void MainWindow::on_keyListWidget_itemChanged(QListWidgetItem *item)
{
	QString newKey = item->text();
	QString oldKey = item->data(Qt::UserRole).toString();

	if (newKey == oldKey)
		return;

    QList<QListWidgetItem*> similarItem = this->ui->keyListWidget->findItems(newKey, Qt::MatchFlag::MatchExactly);

    if (similarItem.count() > 1)
	{
		this->PrintStatusError("Rename Fail : The key '" + newKey + "' already exist !");

		item->setText(oldKey);
	}
	else
	{
		QUndoCommand* renameCommand = new RenameCommand(this, item, oldKey, newKey);
		this->undoStack->push(renameCommand);
	}
}

void MainWindow::on_keyListWidget_customContextMenuRequested(const QPoint &pos)
{
	QMenu* menu = new QMenu(this);

	QAction* action;

	action = menu->addAction("New");
	action->setShortcut(QKeySequence::New);
	action->setEnabled(this->ui->keyListWidget->selectedItems().count() <= 1);

	action = menu->addAction("Rename");
	action->setShortcut(QKeySequence(Qt::Key_F2));
	action->setEnabled(this->ui->keyListWidget->selectedItems().count() == 1);

	action = menu->addAction("Delete");
	action->setShortcut(QKeySequence::Delete);
	action->setEnabled(this->ui->keyListWidget->selectedItems().count() >= 1);

	this->connect(menu, SIGNAL(triggered(QAction*)), this, SLOT(on_keyListWidget_customContextMenuTriggered(QAction*)));
	menu->popup(this->ui->keyListWidget->viewport()->mapToGlobal(pos));
}

void MainWindow::on_keyListWidget_customContextMenuTriggered(QAction* action)
{
	if (action->text() == "New")
		this->NewKey();
	else if (action->text() == "Rename")
		this->RenameKey();
	else if (action->text() == "Delete")
		this->DeleteKey();
}

void MainWindow::on_addKeyButton_clicked()
{
	this->NewKey();
}

void MainWindow::on_renameKeyButton_clicked()
{
	this->RenameKey();
}

void MainWindow::on_deleteKeyButton_clicked()
{
	this->DeleteKey();
}

void MainWindow::NewKey()
{
	if (this->ui->keyListWidget->selectedItems().count() > 1)
		return;

	this->ui->keyListWidget->clearSelection();

	QUndoCommand* addCommand = new AddCommand(this);
	this->undoStack->push(addCommand);
}

KeyListWidgetItem* MainWindow::AddNewKey(const QString& keyName, bool edit)
{
    KeyListWidgetItem* item = new KeyListWidgetItem(keyName, this->ui->keyListWidget);
    item->SetLang(this->project.GetLangList());
	item->setSelected(true);

	this->ui->keyListWidget->sortItems();
	this->ui->keyListWidget->scrollToItem(item);

    if (edit == true)
        this->ui->keyListWidget->editItem(item);

	return item;
}

bool MainWindow::CheckIfItemIsPresentInKeyList(QListWidgetItem* item)
{
	for (int i = 0; i < this->ui->keyListWidget->count(); ++i)
	{
		if (this->ui->keyListWidget->item(i) == item)
			return true;
	}
	return false;
}

bool MainWindow::CheckIfItemIsPresentInKeyList(const QString& itemName)
{
    for (int i = 0; i < this->ui->keyListWidget->count(); ++i)
    {
        if (this->ui->keyListWidget->item(i)->text() == itemName)
            return true;
    }
    return false;
}

QListWidgetItem* MainWindow::GetItemInKeyList(const QString& keyName)
{
	for (int i = 0; i < this->ui->keyListWidget->count(); ++i)
	{
        QListWidgetItem* item = this->ui->keyListWidget->item(i);

		if (item->text() == keyName)
			return item;
	}
	return NULL;
}

void MainWindow::ApplyRenameKey(QListWidgetItem* item, const QString& newName)
{
    QString oldKey = item->text();

	item->setData(Qt::UserRole, newName);
    item->setText(newName);

	this->ui->keyListWidget->sortItems();
	this->ui->keyListWidget->scrollToItem(item);

	this->on_keyListWidget_itemSelectionChanged();
}

void MainWindow::RenameKey()
{
    QList<QListWidgetItem*> selectedKeys = this->ui->keyListWidget->selectedItems();

	if (selectedKeys.count() == 1)
		this->ui->keyListWidget->editItem(selectedKeys[0]);
}

void MainWindow::DeleteKey()
{
	QString text = "Voulez-vous vraiment supprimer les clés suivantes :\n\n";

    QList<QListWidgetItem*> selectedKeys = this->ui->keyListWidget->selectedItems();

	for (int i = 0; i < selectedKeys.count(); ++i)
	{
		text += selectedKeys[i]->text() + "\n";
	}

	if (QMessageBox::warning(this, "Confirmation", text, QMessageBox::Yes, QMessageBox::No) == QMessageBox::Yes)
	{
        QList<QListWidgetItem*> selectedKeys = this->ui->keyListWidget->selectedItems();

		for (int i = 0; i < selectedKeys.count(); ++i)
		{
			this->RemoveKey(selectedKeys[i]);
		}
	}
}

void MainWindow::RemoveKey(QListWidgetItem* item)
{
    int row = this->ui->keyListWidget->row(item);
	this->ui->keyListWidget->takeItem(row);

	if (this->ui->keyListWidget->count() != 0)
	{
		if (row >= this->ui->keyListWidget->count())
			row = this->ui->keyListWidget->count() - 1;

        QListWidgetItem* item = this->ui->keyListWidget->item(row);

		if (item != NULL)
		{
			item->setSelected(true);
			this->ui->keyListWidget->scrollToItem(item);
		}
	}
}

//  === Value Table ===

void MainWindow::RefreshWebView()
{
#ifdef USE_WEBENGINE
	if (this->selectedKeyItem == NULL)
		return;

    QString valueForDefaultLang = this->selectedKeyItem->GetKeyValueByLang(this->defaultLang).GetValue();
	QString lang = this->defaultLang;

	QList<QTableWidgetItem*> selectedList = this->ui->valueTableWidget->selectedItems();

	if (selectedList.size() > 0)
	{
		QTableWidgetItem* item = selectedList[0];

		lang = this->project.GetLangList()[item->row()];
	}

	QString currentLangGoogleTraduc = Singleton<LangHelper>::getInstance().GetLangInfo(lang).googleTraducName;
	QString defaultLangGoogleTraduc = Singleton<LangHelper>::getInstance().GetLangInfo(this->defaultLang).googleTraducName;

	this->webengine->load(QUrl("https://translate.google.com/?ie=UTF-8&hl=fr&client=tw-ob#" + defaultLangGoogleTraduc + "/" + currentLangGoogleTraduc + "/" + valueForDefaultLang.replace(' ', "%20")));
	this->webengine->reload();
#endif
}

void MainWindow::on_valueTableWidget_itemSelectionChanged()
{
	this->RefreshWebView();
}

void MainWindow::on_valueTableWidget_itemChanged(QTableWidgetItem *item)
{
	if (this->selectedKeyItem == NULL)
		return;

	QString value = item->text();
	if (value.contains("  ") == true)
	{
		value.replace("  ", "\n");
		item->setText(value);
		return;
	}

	QString lang = this->project.GetLangList()[item->row()];
    KeyValue& keyValue = this->selectedKeyItem->GetKeyValueByLang(lang);
    keyValue.SetValue(value);

	if (value == "")
	{
        keyValue.SetStatus(KeyStatus::Block);
		this->RefreshButtonStatus(item->row());
        this->selectedKeyItem->RefreshGlobalStatus();
	}
    else if (keyValue.GetStatus() != KeyStatus::Validate)
	{
        keyValue.SetStatus(KeyStatus::ToCheck);
		this->RefreshButtonStatus(item->row());
        this->selectedKeyItem->RefreshGlobalStatus();
	}

	this->ui->valueTableWidget->resizeRowToContents(item->row());
	this->on_valueTableWidget_itemSelectionChanged();
}

//  === Action ===

void MainWindow::on_actionNewProject_triggered()
{
	DialogCreateProject* newProjectDialog = new DialogCreateProject(this, this->project);
	newProjectDialog->show();
}

void MainWindow::on_actionOpenProject_triggered()
{
	QString file = QFileDialog::getOpenFileName(this, "OpenProject");

	if (file == "")
		return;

	this->LoadProject(file);
}

void MainWindow::on_actionCloseProject_triggered()
{
	this->CloseProject();
}

void MainWindow::CloseProject()
{
	this->project.Close();

	this->ui->keyListWidget->clear();
	while (ui->valueTableWidget->rowCount() != 0)
	{
		this->ui->valueTableWidget->removeRow(this->ui->valueTableWidget->rowCount() -1);
	}
	this->ui->searchKeyLineEdit->setText("");

	this->ui->keyListWidget->setEnabled(false);
	this->ui->valueTableWidget->setEnabled(false);
	this->ui->addKeyButton->setEnabled(false);
	this->ui->renameKeyButton->setEnabled(false);
	this->ui->deleteKeyButton->setEnabled(false);
	this->ui->searchKeyLineEdit->setEnabled(false);
}

void MainWindow::on_actionSaveProject_triggered()
{
    QList<KeyListWidgetItem*> allKeyItem;

    for (int i = 0; i < this->ui->keyListWidget->count(); ++i)
        allKeyItem.push_back((KeyListWidgetItem*)this->ui->keyListWidget->item(i));

    this->project.Save(allKeyItem);
}

void MainWindow::on_actionQuit_triggered()
{
	this->close();
}

void MainWindow::on_actionProjectSettings_triggered()
{
	DialogCreateProject* projectSettings = new DialogCreateProject(this, this->project);
	projectSettings->show();
}

void MainWindow::PrintStatusMessage(const QString& message)
{
	ui->statusBar->showMessage(message);
}

void MainWindow::PrintStatusError(const QString& message)
{
	QPalette pal = ui->statusBar->palette();
	pal.setColor(QPalette::WindowText, Qt::red);
	ui->statusBar->setPalette(pal);
	ui->statusBar->showMessage(message);
	pal.setColor(QPalette::WindowText, Qt::black);
	ui->statusBar->setPalette(pal);
}
