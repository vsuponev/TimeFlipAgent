#include "TimeFlipApiClient.h"

#include "Error.h"

#include <QJsonArray>
#include <QJsonDocument>
#include <QNetworkAccessManager>
#include <QNetworkReply>

const QString TIMEFLIP_API_URL = "https://newapi.timeflip.io/api";
const QString TIMEFLIP_API_ENDPOINT_SIGNIN = "auth/email/sign-in";
const QString TIMEFLIP_API_ENDPOINT_TASKS = "tasks/byUser";
const QString TIMEFLIP_API_ENDPOINT_SYNC = "sync";

namespace {

    QString endpointUrl(const QString& endpoint)
    {
        return QString("%1/%2").arg(TIMEFLIP_API_URL, endpoint);
    }

} // anonymous namespace

namespace TimeFlipApi {

TimeFlipApiClient::TimeFlipApiClient(QObject *parent)
    : QObject(parent)
    , m_networkManager(new QNetworkAccessManager(this))
{
}

void TimeFlipApiClient::setCredentials(const Credentials &credentials)
{
    m_credentials = credentials;
}

bool TimeFlipApiClient::isAuthenticated() const
{
    return !m_token.isEmpty();
}

bool TimeFlipApiClient::authenticate()
{
    if (!m_credentials.isValid()) {
        setError("Invalid credentials");
        return false;
    }

    qDebug() << "Authenticating...";

    const QString url = endpointUrl(TIMEFLIP_API_ENDPOINT_SIGNIN);
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    const QJsonDocument doc(m_credentials.toJson());
    const QByteArray data = doc.toJson();

    QNetworkReply *reply = m_networkManager->post(request, data);
    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        handleAuthenticationResponse(reply);
    });

    return true;
}

void TimeFlipApiClient::requestTasks()
{
    if (!isAuthenticated()) {
        setError("Not authenticated");
        return;
    }


    const QString url = endpointUrl(TIMEFLIP_API_ENDPOINT_TASKS);
    QNetworkRequest request(url);
    addAuthorizationHeader(request);

    QNetworkReply *reply = m_networkManager->get(request);
    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        handleTasksResponse(reply);
    });
}

void TimeFlipApiClient::sync()
{
    qDebug() << "Syncing...";
    if (!isAuthenticated()) {
        setError("Not authenticated");
        return;
    }


    const QString url = endpointUrl(TIMEFLIP_API_ENDPOINT_SYNC);
    QNetworkRequest request(url);
    addAuthorizationHeader(request);

    QNetworkReply *reply = m_networkManager->get(request);
    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        handleSyncResponse(reply);
    });
}

QString TimeFlipApiClient::lastError() const
{
    return m_lastError;
}

QByteArray TimeFlipApiClient::token() const
{
    return m_token;
}

UserInfo TimeFlipApiClient::userInfo() const
{
    return m_userInfo;
}

QVector<Task> TimeFlipApiClient::tasks() const
{
    return m_tasks;
}

bool TimeFlipApiClient::checkError(QNetworkReply *reply, const ResponseResult &result)
{
    if (reply->error() == QNetworkReply::NoError || result.httpCode == 200) {
        return false;
    }

    const ResponseResult responseResult = result.isValid() ? result : ResponseResult::fromReply(reply);

    const Error error = Error::fromJson(responseResult.json.object());
    if (error.isValid()) {
        setError(error.message);
        return true;
    }

    setError(reply->errorString());
    return true;
}

void TimeFlipApiClient::handleAuthenticationResponse(QNetworkReply *reply)
{
    auto guard = qScopeGuard([reply]() {
        reply->deleteLater();
    });

    const ResponseResult result = ResponseResult::fromReply(reply);

    if (checkError(reply, result)) {
        return;
    }

    const QJsonObject userObject = result.json["user"].toObject();
    m_userInfo = UserInfo::fromJson(userObject);
    m_token = reply->rawHeader("token");
    emit authenticated(m_userInfo);
}

void TimeFlipApiClient::handleTasksResponse(QNetworkReply *reply)
{
    auto guard = qScopeGuard([reply]() {
        reply->deleteLater();
    });

    const ResponseResult result = ResponseResult::fromReply(reply);

    if (checkError(reply, result)) {
        return;
    }

    m_tasks.clear();
    const QJsonArray tasksArray = result.json.array();
    for (const auto &taskValue : tasksArray) {
        const QJsonObject taskObject = taskValue.toObject();
        m_tasks.append(Task::fromJson(taskObject));
    }
    qDebug() << m_tasks.count() << "tasks received";
    emit tasksReceived();
}

void TimeFlipApiClient::handleSyncResponse(QNetworkReply *reply)
{
    qDebug() << "Sync response received";
    auto guard = qScopeGuard([reply]() {
        reply->deleteLater();
    });

    const ResponseResult result = ResponseResult::fromReply(reply);

    if (checkError(reply, result)) {
        return;
    }

    qDebug() << result.data;
}

void TimeFlipApiClient::setError(const QString &message)
{
    qDebug() << "Error:" << message;
    m_lastError = message;
    emit error(message);
}

void TimeFlipApiClient::addAuthorizationHeader(QNetworkRequest &request)
{
    if (m_token.isEmpty()) {
        return;
    }

    const QByteArray authorization = "Bearer " + m_token;
    request.setRawHeader("Authorization", authorization);
}

} // namespace TimeFlipApi
