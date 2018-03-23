#include "KeyListWidgetItem.h"

KeyListWidgetItem::KeyListWidgetItem(const QString &text, QListWidget *parent) : QListWidgetItem(text, parent)
{
    this->setFlags(this->flags() | Qt::ItemIsEditable);

    QIcon icon;
    icon.addFile(":" + KeyValue::StatusToString(KeyStatus::Block));
}

void KeyListWidgetItem::SetLang(QList<QString> langList)
{
    // Suppression des langues en trop
    QMap<QString, KeyValue>::iterator it = this->langToKeyValueMap.begin();

    while (it != this->langToKeyValueMap.end())
    {
        if (langList.contains(it.key()) == false)
        {
            it = this->langToKeyValueMap.erase(it);
        }
        else
        {
            ++it;
        }
    }

    // Ajout des langues manquantes
    for (auto it = langList.begin(); it != langList.end(); ++it)
    {
        if (this->langToKeyValueMap.contains(*it) == false)
        {
            this->langToKeyValueMap.insert(*it, KeyValue());
        }
    }
}

KeyValue& KeyListWidgetItem::GetKeyValueByLang(const QString& lang)
{
    return this->langToKeyValueMap[lang];
}

void KeyListWidgetItem::RefreshGlobalStatus()
{
    bool containEmpty = false;
    bool containToCheck = false;
    bool containAllValidate = true;

    for (auto it = this->langToKeyValueMap.begin(); it != this->langToKeyValueMap.end(); ++it)
    {
        KeyStatus status = it->GetStatus();

        if (status != KeyStatus::Validate)
            containAllValidate = false;

        if (status == KeyStatus::ToCheck)
            containToCheck = true;
        else if (status == KeyStatus::Block)
            containEmpty = true;
    }

    QString iconName;

    if (containAllValidate == true)
        iconName = KeyValue::StatusToString(KeyStatus::Validate);
    else if (containEmpty == true)
        iconName = KeyValue::StatusToString(KeyStatus::Block);
    else if (containToCheck == true)
        iconName = KeyValue::StatusToString(KeyStatus::ToCheck);
    else
        iconName = "";

    QIcon icon;
    icon.addFile(":" + iconName);

    this->setIcon(icon);
}
