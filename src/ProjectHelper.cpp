#include "ProjectHelper.h"

#include <QIODevice>
#include <QFile>
#include <QFileInfo>
#include <QFileDialog>
#include <QMessageBox>
#include <QErrorMessage>
#include <QStandardPaths>
#include <QXmlStreamReader>
#include <QXmlStreamWriter>

#include "keyvalue.h"
#include "keylistwidgetitem.h"

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
    return this->langList;
}

bool ProjectHelper::Load(const QString& path)
{
	QFile file(path);
	if (file.open(QIODevice::ReadOnly) == false)
	{
		QMessageBox::critical(this->mainWindow, "Erreur", "Impossible d'ouvrir le projet");
		return false;
	}

    this->langList.clear();

	QFileInfo fileInfo(file);

	this->dirPath = fileInfo.absolutePath();
    this->projectName = fileInfo.baseName();

    //QStringList supportedLang;

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

                //supportedLang.push_back(lang);
                this->langList.push_back(lang);
                //this->LoadLang(fileInfo.absolutePath() + "/" + lang + ".lang");
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
    QString configLocation = QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation);

	if (configLocation.isEmpty() == true)
		return false;

	QDir configDir(configLocation);

	if (configDir.exists() == false)
	{
		if (configDir.mkpath(".") == false)
			return false;
	}

    QFile file(configDir.absolutePath() + "/" + "preference.cfg");

    if (file.exists() == false)
    {
        if (file.open(QIODevice::WriteOnly) == false)
            return false;
    }

	QXmlStreamWriter stream(&file);
	stream.setAutoFormatting(true);
    //stream.writeStartDocument();
    stream.writeStartElement("Pref");
    stream.writeAttribute("LastProject", path);
	stream.writeEndElement();
	stream.writeEndDocument();
    file.flush();
	file.close();

	return true;
}

QMap<QString, KeyValue> ProjectHelper::LoadLang(const QString& lang)
{
    QMap<QString, KeyValue> keyToKeyValueMap;

    QString path = this->dirPath + "/" + lang + ".lang";

	QFile file(path);
	if (file.open(QIODevice::ReadOnly) == false)
	{
		file.close();
		QMessageBox::critical(this->mainWindow, "Erreur", "Impossible d'ouvrir en lecture le fichier de lang");
        return keyToKeyValueMap;
	}

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

                keyToKeyValueMap[key].SetValue(value);
                keyToKeyValueMap[key].SetStatus(status);
			}
		}
	}

	file.close();

    return keyToKeyValueMap;
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
}

bool ProjectHelper::Save(QList<KeyListWidgetItem*> keyItemList)
{
    for (int i = 0; i < this->langList.count(); ++i)
	{
        if (this->SaveLang(this->langList[i], keyItemList) == false)
			return false;
	}
	return true;
}

bool ProjectHelper::SaveLang(const QString& lang, QList<KeyListWidgetItem*> keyItemList)
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

    for (int i = 0; i < keyItemList.count(); ++i)
	{
        KeyListWidgetItem* currentKeyItem = keyItemList[i];
        const KeyValue& keyVal = currentKeyItem->GetKeyValueByLang(lang);

		stream.writeStartElement("Loca");
        stream.writeAttribute("Key", currentKeyItem->text());
        stream.writeAttribute("Value", keyVal.GetValue());
        stream.writeAttribute("Status", QString::number((int)(keyVal.GetStatus())));
		stream.writeEndElement();

        if (lang == this->defaultLang && keyVal.GetValue() == "")
		{
			warning = true;
            keyEmpty = currentKeyItem->text();
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
