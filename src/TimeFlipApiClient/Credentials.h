#pragma once

#include <QString>
#include <qjsonobject.h>

namespace TimeFlipApi {

struct Credentials {
    QString email;
    QString password;

    bool isValid() const
    {
        return !email.isEmpty() && !password.isEmpty();
    }

    QJsonObject toJson() const
    {
        QJsonObject object;
        object["email"] = email;
        object["password"] = password;
        return object;
    }
};

} // namespace TimeFlipApi
