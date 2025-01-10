#pragma once

#include <QString>
#include <qjsonobject.h>

namespace TimeFlipApi {

struct Task {
    int id = -1;
    QString name;
    QString description;
    QString color;
    QString icon;
    bool pomodoro = false;

    bool isValid() const
    {
        return id != -1;
    }

    static Task fromJson(const QJsonObject &object)
    {
        Task task;
        task.id = object["id"].toInt();
        task.name = object["name"].toString();
        task.description = object["description"].toString();
        task.color = object["color"].toString();
        task.icon = object["icon"].toString();
        task.pomodoro = object["pomodoro"].toBool();

        return task;
    }
};

} // namespace TimeFlipApi
