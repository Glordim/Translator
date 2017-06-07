#include "ProjectHelper.h"

#include <QFile>
#include <QtXml>
#include <QFileInfo>
#include <QFileDialog>
#include <QMessageBox>
#include <QErrorMessage>

ProjectHelper::ProjectHelper(QWidget* mainWindow) :
	mainWindow(mainWindow)
{
	this->Close();
}

ProjectHelper::~ProjectHelper()
{

}

QString ProjectHelper::GetDirPath() const
{
	return this->dirPath;
}

QString ProjectHelper::GetName() const
{
	return this->projectName;
}

QString ProjectHelper::GetDefaultLang() const
{
	return this->defaultLang;
}

QList<QString>  ProjectHelper::GetLangList() const
{
	return this->allData.keys();
}

QList<QString>  ProjectHelper::GetKeyList(const QString& langName) const
{
	return this->allData[langName].keys();
}

QString ProjectHelper::GetValue(const QString& langName, const QString& keyName) const
{
	return this->allData[langName][keyName].value;
}

void ProjectHelper::SetValue(const QString& langName, const QString& keyName, const QString& value)
{
	this->allData[langName][keyName].value = value;
}

enum KeyStatus ProjectHelper::GetStatus(const QString& langName, const QString& keyName) const
{
	return this->allData[langName][keyName].status;
}

void ProjectHelper::SetStatus(const QString& langName, const QString& keyName, enum KeyStatus status)
{
	this->allData[langName][keyName].status = status;
}

bool ProjectHelper::ContainsKey(const QString& keyName) const
{
	return this->allData[this->defaultLang].contains(keyName);
}

void ProjectHelper::RemoveKey(const QString& keyName)
{
	QList<QString> langList = this->allData.keys();

	int langCount = langList.count();
	for (int i = 0; i < langCount; ++i)
	{
		this->allData[langList[i]].remove(keyName);
	}
}

void ProjectHelper::RenameKey(const QString& oldKeyName, const QString& newKeyName)
{
	QList<QString> langList = this->allData.keys();

	int langCount = langList.count();
	for (int i = 0; i < langCount; ++i)
	{
		QString lang = langList[i];

		QString value = this->allData[lang][oldKeyName].value;
		this->allData[lang].remove(oldKeyName);
		this->allData[lang][newKeyName].value = value;
	}
}

bool ProjectHelper::Load(const QString& path)
{
	QFile file(path);
	if (file.open(QIODevice::ReadOnly) == false)
	{
		file.close();
		QMessageBox::critical(this->mainWindow, "Erreur", "Impossible d'ouvrir le projet");
		return false;
	}

	QFileInfo fileInfo(file);

	QDomDocument dom(file.fileName());

	if (dom.setContent(&file) == false)
	{
		file.close();
		return false;
	}

	file.close();

	this->dirPath = fileInfo.absolutePath();
	this->projectName = fileInfo.baseName();
	this->allData.clear();

	QStringList supportedLang;

	QDomElement docElem = dom.documentElement();
	QDomNode node = docElem.firstChild();

	while (node.isNull() == false)
	{
		QDomElement element = node.toElement();

		if (element.tagName() == "Lang")
		{
			QString lang = element.attribute("Name");
			QString isDefault = element.attribute("IsDefault");

			if (isDefault == "True")
				this->defaultLang = lang;

			supportedLang.push_back(lang);
			this->allData[lang].clear();
			this->LoadLang(fileInfo.absolutePath() + "/" + lang + ".lang");
		}

		node = node.nextSibling();
	}

	QString configLocation = QStandardPaths::writableLocation(QStandardPaths::ConfigLocation);

	if (configLocation.isEmpty() == true)
		return true;

	QDir configDir(configLocation);

	if (configDir.exists() == false)
	{
		if (configDir.mkpath(".") == false)
			return true;
	}

	QFile configFile(configDir.absolutePath() + "/" + "preference.cfg");

	dom.clear();
	QDomElement dom_element = dom.createElement("config");

	QDomElement lang_element = dom.createElement("Line");
	lang_element.setAttribute("LastProject", path);
	dom_element.appendChild(lang_element);
	dom.appendChild(dom_element);

	if (configFile.open(QIODevice::WriteOnly) == false)
	{
		QMessageBox::critical(this->mainWindow, "Erreur", "Impossible d'écrire dans le document XML");
		return true;
	}

	QTextStream stream(&configFile);
	stream << dom.toString();
	configFile.close();

	this->isOpen = true;

	return true;
}

bool ProjectHelper::LoadLang(const QString& path)
{
	QFile file(path);
	if (file.open(QIODevice::ReadOnly) == false)
	{
		file.close();
		//QMessageBox::critical(this->mainWindow, "Erreur", "Impossible d'ouvrir en lecture le fichier de lang");
		return false;
	}

	QFileInfo fileInfo(file);

	QDomDocument dom(file.fileName());

	if (dom.setContent(&file) == false)
	{
		file.close();
		return false;
	}

	file.close();

	QDomElement docElem = dom.documentElement();

	QString lang = fileInfo.baseName();

	QDomNode node = docElem.firstChild();

	while (node.isNull() == false)
	{
		QDomElement element = node.toElement();

		if (element.tagName() == "Loca")
		{
			QString key = element.attribute("Key");
			QString value = element.attribute("Value");
			KeyStatus status = (KeyStatus)element.attribute("Status").toInt();

			this->allData[lang][key].value = value;
			this->allData[lang][key].status = status;
		}

		node = node.nextSibling();
	}

	return true;
}

bool ProjectHelper::IsOpen() const
{
	return this->isOpen;
}

void ProjectHelper::Close()
{
	this->isOpen = false;

	this->projectName = "";
	this->dirPath = "";
	this->defaultLang = "";
	this->allData.clear();
}

bool ProjectHelper::Save()
{
	for (int i = 0; i < this->allData.keys().count(); ++i)
	{
		if (this->SaveLang(this->allData.keys()[i]) == false)
			return false;
	}
	return true;
}

bool ProjectHelper::SaveLang(const QString& lang)
{
	QDomDocument dom;
/*
	QDomProcessingInstruction xmlProcessingInstruction = dom->createProcessingInstruction("xml", "version=\"1.0\"");
	dom->appendChild(xmlProcessingInstruction);
*/
	QDomElement dom_element = dom.createElement(lang);
	//dom_element.setAttribute("Name", projectName);

	bool warning = false;
	QString keyEmpty;

	QMap<QString, KeyValue> keyValueMap = this->allData[lang];
	QList<QString> keyList = keyValueMap.keys();

	int keyCount = keyList.count();
	for (int i = 0; i < keyCount; ++i)
	{
		QString currentKey = keyList[i];
		const KeyValue& keyVal = keyValueMap[currentKey];

		QDomElement lang_element = dom.createElement("Loca");
		lang_element.setAttribute("Key", currentKey);
		lang_element.setAttribute("Value", keyVal.value);
		lang_element.setAttribute("Status", (int)(keyVal.status));
		if (lang == this->defaultLang && keyVal.value == "")
		{
			warning = true;
			keyEmpty = currentKey;
		}
		dom_element.appendChild(lang_element);
	}

	dom.appendChild(dom_element);

	QString filePath = this->dirPath + "/" + lang + ".lang";
	QFile file(filePath);
	if (file.open(QIODevice::WriteOnly) == false)
	{
		QMessageBox::critical(this->mainWindow, "Erreur", "Impossible d'écrire dans le document XML");
		return false;
	}

	QTextStream stream(&file);
	stream << dom.toString();
	file.close();

	if (warning == true)
		QMessageBox::warning(this->mainWindow, "Warning", "La langue par default contient des champs vide !\n(Key = " + keyEmpty + " )");

	return true;
}
