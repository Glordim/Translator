#include "DialogCreateProject.h"
#include "ui_DialogCreateProject.h"

#include "MainWindow.h"

#include <QCloseEvent>

#include <QFileDialog>
#include <QMessageBox>
#include <QErrorMessage>

#include <QtXml>

#include "Singleton.h"
#include "LangHelper.h"

DialogCreateProject::DialogCreateProject(QWidget *parent, ProjectHelper& project) :
	QDialog(parent),
	ui(new Ui::DialogCreateProject),
	project(project)
{
	ui->setupUi(this);

	LangHelper& langHelper = Singleton<LangHelper>::getInstance();

	QList<QString> list = langHelper.GetKeyList();
	/*
	list.push_back("Afrikaans");
	list.push_back("Arabic");
	list.push_back("Basque");
	list.push_back("Belarusian");
	list.push_back("Czech");
	list.push_back("Danish");
	list.push_back("Dutch");
	list.push_back("English");
	list.push_back("Estonian");
	list.push_back("Faroese");
	list.push_back("Finnish");
	list.push_back("French");
	list.push_back("German");
	list.push_back("Hebrew");
	list.push_back("Icelandic");
	list.push_back("Indonesian");
	list.push_back("Italian");
	list.push_back("Japanese");
	list.push_back("Korean");
	list.push_back("Latvian");
	list.push_back("Lithuanian");
	list.push_back("Norwegian");
	list.push_back("Polish");
	list.push_back("Portuguese");
	list.push_back("Romanian");
	list.push_back("Russian");
	list.push_back("Serbo-Croatian");
	list.push_back("Slovak");
	list.push_back("Slovenian");
	list.push_back("Spanish");
	list.push_back("Swedish");
	list.push_back("Thai");
	list.push_back("Turkish");
	list.push_back("Ukrainian");
	list.push_back("Hungarian");
	list.push_back("Vietnamese");
*/
	ui->comboBox->addItems(list);

	ui->listWidget->addItems(list);
	ui->listWidget_2->addItems(list);

	for (int i = 0; i < ui->listWidget_2->count(); ++i)
		ui->listWidget_2->item(i)->setHidden(true);

	if (this->project.IsOpen() == true)
	{
		ui->lineEdit_2->setEnabled(false);
		ui->lineEdit_2->setText(this->project.GetName());

		ui->lineEdit->setEnabled(false);

		QDir dir = QDir(this->project.GetDirPath());
		dir.cdUp();

		ui->lineEdit->setText(dir.path());
		ui->toolButton->setEnabled(false);

		ui->comboBox->setCurrentIndex(list.indexOf(this->project.GetDefaultLang()));

		QList<QString> langList = this->project.GetLangList();
		int langCount = langList.count();

		for (int i = 0; i < langCount; ++i)
		{
			QListWidgetItem* item = this->ui->listWidget->item(list.indexOf(langList[i]));

			this->MoveAllToSupported(item);
		}
	}
	else
	{
		ui->lineEdit->setEnabled(true);
		ui->lineEdit_2->setEnabled(true);
		ui->toolButton->setEnabled(true);

		ui->comboBox->setCurrentIndex(list.indexOf("English"));
	}
}

DialogCreateProject::~DialogCreateProject()
{
	delete ui;
}

void DialogCreateProject::on_toolButton_clicked()
{
	// QFileDialog* fileDialog = new QFileDialog(this);

	//fileDialog->set
	//fileDialog->setFileMode(QFileDialog::Directory);

	QString dir = QFileDialog::getExistingDirectory(this, "Project Location");
	this->ui->lineEdit->setText(dir);
	//fileDialog->setFilter(QFileDialog::ShowDirsOnly);

	//fileDialog->show();
}

void DialogCreateProject::on_ButtonAllToSupported_clicked()
{
	if (this->ui->listWidget->selectedItems().empty())
		return ;

	this->MoveAllToSupported(this->ui->listWidget->selectedItems()[0]);
}

void DialogCreateProject::on_listWidget_itemDoubleClicked(QListWidgetItem *item)
{
	this->MoveAllToSupported(item);
}

void DialogCreateProject::MoveAllToSupported(QListWidgetItem *item)
{
	item->setHidden(true);
	int row = this->ui->listWidget->row(item);
	this->ui->listWidget_2->item(row)->setHidden(false);
}

void DialogCreateProject::on_ButtonSupportedToAll_clicked()
{
	if (this->ui->listWidget_2->selectedItems().empty())
		return ;

	this->MoveSupportedToAll(this->ui->listWidget_2->selectedItems()[0]);
}

void DialogCreateProject::on_listWidget_2_itemDoubleClicked(QListWidgetItem *item)
{
	this->MoveSupportedToAll(item);
}

void DialogCreateProject::MoveSupportedToAll(QListWidgetItem *item)
{
	item->setHidden(true);
	int row = this->ui->listWidget_2->row(item);
	this->ui->listWidget->item(row)->setHidden(false);
}

void DialogCreateProject::done(int r)
{
	if (QDialog::Accepted == r)
	{
		QString projectName = this->ui->lineEdit_2->text();

		if (projectName == "")
		{
			QMessageBox messageBox;
			messageBox.critical(0,"Error","Project name empty !");
			return ;
		}
		QDir dir(this->ui->lineEdit->text());
		if (dir.exists() == false || this->ui->lineEdit->text() == "")
		{
			QMessageBox messageBox;
			messageBox.critical(0,"Error","Project location invalid !");
			return ;
		}

		QString defaultLang = this->ui->comboBox->currentText();

		QStringList supportedLang;

		for (int i = 0; i < ui->listWidget_2->count(); ++i)
		{
			if (ui->listWidget_2->item(i)->isHidden() == false)
				supportedLang.push_back(ui->listWidget_2->item(i)->text());
		}

		if (supportedLang.contains(defaultLang) == false)
		{
			QMessageBox messageBox;
			messageBox.critical(0,"Error","Default Lang is not in Supported Lang !");
			return ;
		}

		if (this->project.IsOpen() == true)
		{
			QList<QString> langList = this->project.GetLangList();
			int langCount = langList.count();

			QList<QString> deleteList;

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
				QFile::remove(dir.absolutePath() + "/" + projectName + "/" + lang + ".lang");
		}
		else
		{
			if (dir.mkdir(projectName) == false)
			{
				QMessageBox messageBox;
				messageBox.critical(0,"Error","Create Project Dir Failed !");
				return ;
			}
		}

		QDomDocument* dom = new QDomDocument(projectName + ".trans");
		QDomElement dom_element = dom->createElement("Project");
		dom_element.setAttribute("Name", projectName);

		for (int i = 0; i < supportedLang.count(); ++i)
		{
			QDomElement lang_element = dom->createElement("Lang");
			lang_element.setAttribute("Name", supportedLang[i]);
			if (supportedLang[i] == defaultLang)
				lang_element.setAttribute("IsDefault", "True");
			dom_element.appendChild(lang_element);
		}

		dom->appendChild(dom_element);

		QString filePath = dir.absolutePath() + "/" + projectName + "/" + projectName + ".trans";
		QFile file(filePath);
		if (file.open(QIODevice::WriteOnly) == false)
		{
			file.close();
			QMessageBox::critical(this,"Erreur","Impossible d'écrire dans le document XML");
			return;
		}

		QTextStream stream(&file);
		stream << dom->toString();
		file.close();

		QObject* obj = this;

		while (obj->parent() != NULL)
			obj = obj->parent();

		MainWindow* main = static_cast<MainWindow*>(obj);

		main->LoadProject(filePath);

		QDialog::done(r);
	}
	else
		QDialog::done(r);
}

