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

	QStringList GetKeyList(void) const;

	QString GetDisplayName(const QString& key) const;
	QString GetSavedName(const QString& key) const;
	QString GetGoogleTraducName(const QString& key) const;

private:
	QMap<QString, LangInfo> langMap;
};

#endif // LANGHELPER_H
