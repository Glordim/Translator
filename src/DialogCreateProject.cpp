#include "Singleton.h"
#include "LangHelper.h"
#include "MainWindow.h"
#include "DialogCreateProject.h"
#include "ui_DialogCreateProject.h"

#include <QtXml>
#include <QCloseEvent>
#include <QFileDialog>
#include <QMessageBox>
#include <QErrorMessage>

DialogCreateProject::DialogCreateProject(QWidget *parent, ProjectHelper& project) :
	QDialog(parent),
	ui(new Ui::DialogCreateProject),
	project(project)
{
	ui->setupUi(this);

	LangHelper& langHelper = Singleton<LangHelper>::getInstance();

	QList<QString> list = langHelper.GetKeyList();

	ui->defaultLangComboBox->addItems(list);

	ui->availableListWidget->addItems(list);
	ui->supportedListWidget->addItems(list);

	int supportedCount = ui->supportedListWidget->count();
	for (int i = 0; i < supportedCount; ++i)
	{
		ui->supportedListWidget->item(i)->setHidden(true);
	}

	if (this->project.IsOpen() == true)
	{
		ui->projectNameLineEdit->setEnabled(false);
		ui->projectNameLineEdit->setText(this->project.GetName());

		QDir dir = QDir(this->project.GetDirPath());

		ui->projectLocationLineEdit->setText(dir.path());
		ui->projectLocationLineEdit->setEnabled(false);
		ui->projectLocationFindButton->setEnabled(false);

		ui->defaultLangComboBox->setCurrentIndex(list.indexOf(this->project.GetDefaultLang()));

		QList<QString> langList = this->project.GetLangList();

		int langCount = langList.count();
		for (int i = 0; i < langCount; ++i)
		{
			QListWidgetItem* item = this->ui->availableListWidget->item(list.indexOf(langList[i]));

			this->MoveToSupported(item);
		}
	}
	else
	{
		ui->projectNameLineEdit->setEnabled(true);
		ui->projectLocationLineEdit->setEnabled(true);
		ui->projectLocationFindButton->setEnabled(true);

		ui->defaultLangComboBox->setCurrentIndex(list.indexOf("English"));
	}
}

DialogCreateProject::~DialogCreateProject()
{
	delete ui;
}

void DialogCreateProject::on_projectLocationFindButton_clicked()
{
	QString dir = QFileDialog::getExistingDirectory(this, "Project Location");
	this->ui->projectLocationLineEdit->setText(dir);
}

void DialogCreateProject::on_moveToSupportedButton_clicked()
{
	if (this->ui->availableListWidget->selectedItems().empty() == true)
		return ;

	this->MoveToSupported(this->ui->availableListWidget->selectedItems()[0]);
}

void DialogCreateProject::on_availableListWidget_itemDoubleClicked(QListWidgetItem *item)
{
	this->MoveToSupported(item);
}

void DialogCreateProject::MoveToSupported(QListWidgetItem *item)
{
	item->setHidden(true);
	int row = this->ui->availableListWidget->row(item);
	this->ui->supportedListWidget->item(row)->setHidden(false);
}

void DialogCreateProject::on_moveToAvailableButton_clicked()
{
	if (this->ui->supportedListWidget->selectedItems().empty() == true)
		return ;

	this->MoveToAvailable(this->ui->supportedListWidget->selectedItems()[0]);
}

void DialogCreateProject::on_supportedListWidget_itemDoubleClicked(QListWidgetItem *item)
{
	this->MoveToAvailable(item);
}

void DialogCreateProject::MoveToAvailable(QListWidgetItem *item)
{
	item->setHidden(true);
	int row = this->ui->supportedListWidget->row(item);
	this->ui->availableListWidget->item(row)->setHidden(false);
}

void DialogCreateProject::done(int r)
{
	if (QDialog::Accepted == r)
	{
		QString projectName = this->ui->projectNameLineEdit->text();

		if (projectName == "")
		{
			QMessageBox::critical(0, "Error", "Project name empty !");
			return ;
		}
		QDir dir(this->ui->projectLocationLineEdit->text());
		if (dir.exists() == false || this->ui->projectLocationLineEdit->text() == "")
		{
			QMessageBox::critical(0, "Error", "Project location invalid !");
			return ;
		}

		QString defaultLang = this->ui->defaultLangComboBox->currentText();

		QStringList supportedLang;

		int supportedCount = ui->supportedListWidget->count();
		for (int i = 0; i < supportedCount; ++i)
		{
			if (ui->supportedListWidget->item(i)->isHidden() == false)
				supportedLang.push_back(ui->supportedListWidget->item(i)->text());
		}

		if (supportedLang.contains(defaultLang) == false)
		{
			QMessageBox::critical(0, "Error", "Default Lang is not in Supported Lang !");
			return ;
		}

		if (this->project.IsOpen() == true)
		{
			QList<QString> langList = this->project.GetLangList();
			QList<QString> deleteList;

			int langCount = langList.count();
			for (int i = 0; i < langCount; ++i)
			{
				if (supportedLang.contains(langList[i]) == false)
				{
					if (QMessageBox::question(this, "Warning", "Attention la langue " + langList[i] + " a ete retirer de la liste des langues supportees, toutes ses valeurs disparaitront.\n\nVoulez vous continuer ?", QMessageBox::Yes|QMessageBox::No) == QMessageBox::No)
						return;

					deleteList.push_back(langList[i]);
				}
			}

			foreach (QString lang, deleteList)
			{
				QFile::remove(dir.absolutePath() + "/" + lang + ".lang");
			}
		}
		else
		{
			if (dir.mkdir(projectName) == false)
			{
				QMessageBox::critical(0, "Error", "Create Project Dir Failed !");
				return ;
			}
		}

		QDomDocument dom;
		QDomElement dom_element = dom.createElement("Project");
		dom_element.setAttribute("Name", projectName);

		supportedCount = supportedLang.count();
		for (int i = 0; i < supportedCount; ++i)
		{
			QDomElement lang_element = dom.createElement("Lang");
			lang_element.setAttribute("Name", supportedLang[i]);

			if (supportedLang[i] == defaultLang)
				lang_element.setAttribute("IsDefault", "True");

			dom_element.appendChild(lang_element);
		}

		dom.appendChild(dom_element);

		QString filePath = dir.absolutePath() + "/" + projectName + ".trans";
		QFile file(filePath);
		if (file.open(QIODevice::WriteOnly) == false)
		{
			QMessageBox::critical(this, "Erreur", "Impossible d'écrire dans le document XML");
			return;
		}

		QTextStream stream(&file);
		stream << dom.toString();
		file.close();

		QObject* obj = this;

		while (obj->parent() != NULL)
		{
			obj = obj->parent();
		}

		MainWindow* main = static_cast<MainWindow*>(obj);

		main->LoadProject(filePath);

		QDialog::done(r);
	}
	else
	{
		QDialog::done(r);
	}
}

