#include "LangHelper.h"

#include <QFile>
#include <QtXml>
#include <QFileInfo>

#include <QFileDialog>
#include <QMessageBox>
#include <QErrorMessage>

#include "Singleton.h"
#include "MainWindow.h"

LangHelper::LangHelper(void)
{

}

bool LangHelper::Load(const QString& path)
{
	LangHelper::langMap.clear();

	QFileInfo root(".");

	QDebug(QtInfoMsg) << root.absolutePath();

	QFileInfo fileInfo(path);

	QFile file(path);
	if (file.open(QIODevice::ReadOnly) == false)
	{
		file.close();

		QMessageBox::critical(NULL,"Erreur","Impossible d'ouvrir en lecture le descriptif des langues");

		return false;
	}

	QDomDocument* dom = new QDomDocument(fileInfo.fileName());

	if (dom->setContent(&file) == false)
	{
		file.close();
		return false;
	}

	file.close();

	QDomElement docElem = dom->documentElement();

	QDomNode n = docElem.firstChild();

	while (n.isNull() == false)
	{
		QDomElement e = n.toElement();

		if (e.tagName() == "LangInfo")
		{
			LangInfo lang;

			lang.displayName = e.attribute("Display");
			lang.savedName = e.attribute("Saved");
			lang.googleTraducName = e.attribute("GoogleTraduc");

			this->langMap[lang.displayName] = lang;
		}

		n = n.nextSibling();
	}

	return true;
}

QStringList LangHelper::GetKeyList() const
{
	QStringList list(this->langMap.keys());
	return list;
}

QString LangHelper::GetDisplayName(const QString& key) const
{
	if (this->langMap.contains(key) == true)
		return this->langMap[key].displayName;
	return "Key Not Found";
}

QString LangHelper::GetSavedName(const QString& key) const
{
	if (this->langMap.contains(key) == true)
		return this->langMap[key].savedName;
	return "Key Not Found";
}

QString LangHelper::GetGoogleTraducName(const QString& key) const
{
	if (this->langMap.contains(key) == true)
		return this->langMap[key].googleTraducName;
	return "Key Not Found";
}
