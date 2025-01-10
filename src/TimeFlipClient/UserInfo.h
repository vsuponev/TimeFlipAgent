#pragma once

#include <QJsonObject>
#include <QString>

namespace TimeFlipApi {

struct UserInfo {
    int id = -1;
    QString email;
    QString fullName;
    bool emailConfirmed = false;

    bool isValid() const
    {
        return id != -1;
    }

    static UserInfo fromJson(const QJsonObject &object)
    {
        UserInfo user;
        user.id = object["id"].toInt();
        user.email = object["email"].toString();
        user.fullName = object["fullName"].toString();
        user.emailConfirmed = object["emailConfirmed"].toBool();

        return user;
    }
};

} // namespace TimeFlipApi
