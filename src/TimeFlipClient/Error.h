#pragma once

#include <QString>
#include <qjsonobject.h>

namespace TimeFlipApi {

struct Error {
    int status = -1;
    QString message;

    bool isValid() const
    {
        return status != -1;
    }

    static Error fromJson(const QJsonObject &object)
    {
        Error error;
        error.status = object["status"].toInt();
        error.message = object["message"].toString();

        return error;
    }
};

} // namespace TimeFlipApi
