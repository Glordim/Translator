#ifndef KEYVALUE_H
#define KEYVALUE_H

#include <QString>

enum KeyStatus
{
    Block = 0,
    Validate,
    ToCheck,
    Nb
};

struct KeyValue
{
    KeyValue();

    void SetValue(const QString& value);
    const QString& GetValue() const;

    void SetStatus(KeyStatus status);
    KeyStatus GetStatus() const;

    static QString StatusToString(enum KeyStatus status);

private:
    QString value;
    enum KeyStatus status;
};

#endif // KEYVALUE_H
