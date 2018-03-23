#include "KeyValue.h"

KeyValue::KeyValue()
{
    this->value = "";
    this->status = KeyStatus::Block;
}

void KeyValue::SetValue(const QString& value)
{
    this->value = value;
}

const QString& KeyValue::GetValue() const
{
    return this->value;
}

void KeyValue::SetStatus(KeyStatus status)
{
    this->status = status;
}

KeyStatus KeyValue::GetStatus() const
{
    return this->status;
}

QString KeyValue::StatusToString(enum KeyStatus status)
{
    switch (status)
    {
        case Block:
        {
            return "Error";
        }
        break;

        case Validate:
        {
            return "Ok";
        }
        break;

        case ToCheck:
        {
            return "Warning";
        }
        break;
    }

    return "Error";
}
