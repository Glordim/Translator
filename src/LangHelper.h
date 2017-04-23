#ifndef LANGHELPER_H
#define LANGHELPER_H

#include <QMap>
#include <QString>

class LangHelper
{
private:
	struct LangInfo
	{
		QString displayName;
		QString savedName;
		QString googleTraducName;
	};

public:
	LangHelper(void);
	bool Load(const QString& path);

	QList<QString> GetKeyList(void) const;
	LangInfo GetLangInfo(const QString& key) const;

private:
	QMap<QString, LangInfo> langMap;
};

#endif // LANGHELPER_H
