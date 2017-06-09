#ifndef PROJECTHELPER_H
#define PROJECTHELPER_H

#include <QWidget>
#include <QString>
#include <QMap>

enum KeyStatus
{
	Block = 0,
	Validate,
	ToCheck,
	Nb
};

struct KeyValue
{
	QString value;
	enum KeyStatus status;

	static QString StatusToString(enum KeyStatus status)
	{
		switch (status)
		{
		default:
		case Block:
			return "Error";
			break;
		case Validate:
			return "Ok";
			break;
		case ToCheck:
			return "Warning";
			break;
		}
	}
};

class ProjectHelper
{
public:
	explicit ProjectHelper(QWidget* mainWindow);
	virtual ~ProjectHelper();

	bool            IsOpen() const;
	void            Close();

	QString         GetDirPath() const;
	QString         GetName() const;
	QString         GetDefaultLang() const;
	QList<QString>  GetLangList() const;
	QList<QString>  GetKeyList(const QString& langName) const;

	QString GetValue(const QString& langName, const QString& keyName) const;
	void    SetValue(const QString& langName, const QString& keyName, const QString& value);

	enum KeyStatus GetStatus(const QString& langName, const QString& keyName) const;
	void    SetStatus(const QString& langName, const QString& keyName, enum KeyStatus status);

	bool    ContainsKey(const QString& keyName) const;
	void    RemoveKey(const QString& keyName);
	void    RenameKey(const QString& oldKeyName, const QString& newKeyName);

	bool    Load(const QString& path);
	bool    LoadLang(const QString& path);

	bool    Save();
	bool    SaveLang(const QString& key);

	bool	SetDefaultProjectInConfigFile(const QString& path);

private:

	bool    isOpen;

	QString dirPath;
	QString projectName;

	QString defaultLang;

	QWidget* mainWindow;

	QMap<QString, QMap<QString, KeyValue> > allData;
};

#endif // PROJECTHELPER_H
