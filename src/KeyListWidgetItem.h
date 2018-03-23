#ifndef KEYLISTWIDGETITEM_H
#define KEYLISTWIDGETITEM_H

#include <QMap>
#include <QString>
#include <QListWidgetItem>

#include "KeyValue.h"

class KeyListWidgetItem : public QListWidgetItem
{
public:
    KeyListWidgetItem(const QString &text, QListWidget *parent = Q_NULLPTR);
    void SetLang(QList<QString> langList);

    KeyValue& GetKeyValueByLang(const QString& lang);

    void RefreshGlobalStatus();

private:


    QMap<QString, KeyValue> langToKeyValueMap;
};

#endif // KEYLISTWIDGETITEM_H
