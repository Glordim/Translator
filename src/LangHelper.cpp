#include "LangHelper.h"

#include <QFile>
#include <QtXml>
#include <QMessageBox>

LangHelper::LangHelper(void)
{

}

bool LangHelper::Load(const QString& path)
{
	QFile file(path);
	if (file.open(QIODevice::ReadOnly) == false)
	{
		QMessageBox::critical(NULL, "Error", path + ": " + file.errorString());
		return false;
	}

	QDomDocument dom;

	if (dom.setContent(&file) == false)
	{
		file.close();
		return false;
	}

	file.close();

	this->langMap.clear();

	QDomElement docElem = dom.documentElement();

	QDomNode node = docElem.firstChild();

	while (node.isNull() == false)
	{
		QDomElement element = node.toElement();

		if (element.tagName() == "LangInfo")
		{
			LangInfo lang;

			lang.displayName = element.attribute("Display");
			lang.savedName = element.attribute("Saved");
			lang.googleTraducName = element.attribute("GoogleTraduc");

			this->langMap[lang.displayName] = lang;
		}

		node = node.nextSibling();
	}

	return true;
}

QList<QString> LangHelper::GetKeyList() const
{
	return this->langMap.keys();
}

LangHelper::LangInfo LangHelper::GetLangInfo(const QString& key) const
{
	return this->langMap[key];
}
