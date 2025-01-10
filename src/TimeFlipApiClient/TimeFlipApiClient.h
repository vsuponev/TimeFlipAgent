#pragma once

#include "Credentials.h"
#include "ResponseResult.h"
#include "Task.h"
#include "UserInfo.h"

#include <QByteArray>
#include <QObject>

class QNetworkAccessManager;
class QNetworkReply;

namespace TimeFlipApi {

class TimeFlipApiClient: public QObject
{
    Q_OBJECT
public:
    TimeFlipApiClient(QObject *parent = nullptr);

    void setCredentials(const Credentials& credentials);

    bool isAuthenticated() const;
    bool authenticate();

    void requestTasks();
    void sync();

    QString lastError() const;
    QByteArray token() const;
    UserInfo userInfo() const;
    QVector<Task> tasks() const;

signals:
    void authenticated(const UserInfo &userInfo);
    void tasksReceived();
    void error(const QString &message);

private slots:
    bool checkError(QNetworkReply *reply, const ResponseResult &result = {});

    void handleAuthenticationResponse(QNetworkReply *reply);
    void handleTasksResponse(QNetworkReply *reply);
    void handleSyncResponse(QNetworkReply *reply);

    void setError(const QString &message);

private:
    void addAuthorizationHeader(QNetworkRequest &request);

private:
    QByteArray m_token;
    QString m_lastError;

    QNetworkAccessManager *m_networkManager;

    Credentials m_credentials;
    UserInfo m_userInfo;
    QVector<Task> m_tasks;
};

} // namespace TimeFlipApi
