#include "LangHelper.h"

#include <QFile>
#include <QMessageBox>
#include <QXmlStreamReader>

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

	this->langMap.clear();

	QXmlStreamReader xml(&file);

	while (xml.atEnd() == false)
	{
		xml.readNext();

		if (xml.isStartElement() == true)
		{
			if (xml.name() == "LangInfo")
			{
				QXmlStreamAttributes attributesVec = xml.attributes();

				LangInfo lang;

				for (int i = 0; i < attributesVec.count(); ++i)
				{
					QXmlStreamAttribute attribute = attributesVec[i];
					QString attributeName = attribute.name().toString();

					if (attributeName == "Display")
					{
						lang.displayName = attribute.value().toString();
					}
					else if (attributeName == "Saved")
					{
						lang.savedName = attribute.value().toString();
					}
					else if (attributeName == "GoogleTraduc")
					{
						lang.googleTraducName = attribute.value().toString();
					}
				}

				this->langMap[lang.savedName] = lang;
			}
		}
	}

	file.close();

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
