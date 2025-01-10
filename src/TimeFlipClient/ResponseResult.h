#pragma once

#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkReply>

struct ResponseResult {
    int httpCode = 0;
    QByteArray data;
    QJsonObject json;

    bool isValid() const
    {
        return httpCode != 0;
    }

    static ResponseResult fromReply(QNetworkReply *reply)
    {
        ResponseResult result;
        result.httpCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
        result.data = reply->readAll();
        result.json = QJsonDocument::fromJson(result.data).object();
        return result;
    }
};
