#include "ProjectHelper.h"

#include <QFile>
#include <QFileInfo>
#include <QFileDialog>
#include <QMessageBox>
#include <QErrorMessage>
#include <QStandardPaths>
#include <QXmlStreamReader>
#include <QXmlStreamWriter>

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
		QMessageBox::critical(this->mainWindow, "Erreur", "Impossible d'ouvrir le projet");
		return false;
	}

	QFileInfo fileInfo(file);

	this->dirPath = fileInfo.absolutePath();
	this->projectName = fileInfo.baseName();
	this->allData.clear();

	QStringList supportedLang;

	QXmlStreamReader xml(&file);

	while (xml.atEnd() == false)
	{
		xml.readNext();

		if (xml.isStartElement() == true)
		{
			if (xml.name() == "Lang")
			{
				QXmlStreamAttributes attributesVec = xml.attributes();

				QString lang;

				for (int i = 0; i < attributesVec.count(); ++i)
				{
					QXmlStreamAttribute attribute = attributesVec[i];
					QString attributeName = attribute.name().toString();

					if (attributeName == "Name")
					{
						lang = attribute.value().toString();
					}
					else if (attributeName == "IsDefault")
					{
						if (attribute.value().toString() == "True")
							this->defaultLang = lang;
					}
				}

				supportedLang.push_back(lang);
				this->allData[lang].clear();
				this->LoadLang(fileInfo.absolutePath() + "/" + lang + ".lang");
			}
		}
	}
	if (xml.hasError() == true)
	{

	}

	this->isOpen = true;

	this->SetDefaultProjectInConfigFile(path);

	return true;
}

bool ProjectHelper::SetDefaultProjectInConfigFile(const QString& path)
{
	QString configLocation = QStandardPaths::writableLocation(QStandardPaths::ConfigLocation);

	if (configLocation.isEmpty() == true)
		return false;

	QDir configDir(configLocation);

	if (configDir.exists() == false)
	{
		if (configDir.mkpath(".") == false)
			return false;
	}

	QFile file(configDir.absolutePath() + "/" + "preference.cfg");

	QXmlStreamWriter stream(&file);
	stream.setAutoFormatting(true);
	//stream.writeStartDocument();
	stream.writeStartElement("Config");
	stream.writeStartElement("Loca");
	stream.writeAttribute("LastProject", path);
	stream.writeEndElement();
	stream.writeEndElement();
	stream.writeEndDocument();
	file.close();

	return true;
}

bool ProjectHelper::LoadLang(const QString& path)
{
	QFile file(path);
	if (file.open(QIODevice::ReadOnly) == false)
	{
		file.close();
		QMessageBox::critical(this->mainWindow, "Erreur", "Impossible d'ouvrir en lecture le fichier de lang");
		return false;
	}

	QFileInfo fileInfo(file);
	QString lang = fileInfo.baseName();

	QXmlStreamReader xml(&file);

	while (xml.atEnd() == false)
	{
		xml.readNext();

		if (xml.isStartElement() == true)
		{
			if (xml.name() == "Loca")
			{
				QXmlStreamAttributes attributesVec = xml.attributes();

				QString key;
				QString value;
				KeyStatus status;

				for (int i = 0; i < attributesVec.count(); ++i)
				{
					QXmlStreamAttribute attribute = attributesVec[i];
					QString attributeName = attribute.name().toString();

					if (attributeName == "Key")
					{
						key = attribute.value().toString();
					}
					else if (attributeName == "Value")
					{
						value = attribute.value().toString();
					}
					else if (attributeName == "Status")
					{
						status = (KeyStatus)attribute.value().toInt();
					}
				}

				this->allData[lang][key].value = value;
				this->allData[lang][key].status = status;
			}
		}
	}

	file.close();

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
	QString filePath = this->dirPath + "/" + lang + ".lang";
	QFile file(filePath);
	if (file.open(QIODevice::WriteOnly) == false)
	{
		QMessageBox::critical(this->mainWindow, "Erreur", "Impossible d'écrire dans le document XML");
		return false;
	}

	bool warning = false;
	QString keyEmpty;

	QXmlStreamWriter stream(&file);
	stream.setAutoFormatting(true);
	//stream.writeStartDocument();
	stream.writeStartElement(lang);

	QMap<QString, KeyValue> keyValueMap = this->allData[lang];
	QList<QString> keyList = keyValueMap.keys();

	int keyCount = keyList.count();
	for (int i = 0; i < keyCount; ++i)
	{
		QString currentKey = keyList[i];
		const KeyValue& keyVal = keyValueMap[currentKey];

		stream.writeStartElement("Loca");
		stream.writeAttribute("Key", currentKey);
		stream.writeAttribute("Value", keyVal.value);
		stream.writeAttribute("Status", QString::number((int)(keyVal.status)));
		stream.writeEndElement();

		if (lang == this->defaultLang && keyVal.value == "")
		{
			warning = true;
			keyEmpty = currentKey;
		}
	}

	stream.writeEndElement();
	stream.writeEndDocument();
	file.close();

	if (warning == true)
	{
		QMessageBox::warning(this->mainWindow, "Warning", "La langue par default contient des champs vide !\n(Key = " + keyEmpty + " )");
	}

	return true;
}
