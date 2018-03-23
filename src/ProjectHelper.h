#ifndef PROJECTHELPER_H
#define PROJECTHELPER_H

#include <QWidget>
#include <QString>
#include <QMap>

#include "keylistwidgetitem.h"

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

	bool    Load(const QString& path);
    QMap<QString, KeyValue>    LoadLang(const QString& path);

    bool    Save(QList<KeyListWidgetItem*> keyItemList);
    bool    SaveLang(const QString& lang, QList<KeyListWidgetItem*> keyItemList);

	bool	SetDefaultProjectInConfigFile(const QString& path);

private:

	bool    isOpen;

	QString dirPath;
	QString projectName;

	QString defaultLang;

    QWidget* mainWindow;

    QList<QString> langList;
};

#endif // PROJECTHELPER_H
