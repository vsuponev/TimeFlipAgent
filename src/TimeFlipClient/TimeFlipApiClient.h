#pragma once

#include "Credentials.h"
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

    QString lastError() const;
    QByteArray token() const;
    UserInfo userInfo() const;

signals:
    void authenticated(const UserInfo &userInfo);
    void tasksReceived();
    void error(const QString &message);

private slots:
    void handleAuthenticationResponse(QNetworkReply *reply);
    void handleError(QNetworkReply *reply);

    void setError(const QString &message);

private:
    QByteArray m_token;
    QString m_lastError;

    QNetworkAccessManager *m_networkManager;

    Credentials m_credentials;
    UserInfo m_userInfo;
};

} // namespace TimeFlipApi
