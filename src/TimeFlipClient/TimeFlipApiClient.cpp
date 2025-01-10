#include "TimeFlipApiClient.h"

#include "Error.h"

#include <QJsonDocument>
#include <QNetworkAccessManager>
#include <QNetworkReply>

const QString TIMEFLIP_API_URL = "https://newapi.timeflip.io/api";
const QString TIMEFLIP_API_ENDPOINT_SIGNIN = "auth/email/sign-in";
const QString TIMEFLIP_API_ENDPOINT_TASKS = "report/daily";

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

void TimeFlipApiClient::handleAuthenticationResponse(QNetworkReply *reply)
{
    auto guard = qScopeGuard([reply]() {
        reply->deleteLater();
    });

    if (reply->error() != QNetworkReply::NoError) {
        handleError(reply);
        return;
    }

    const int httpCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    const QByteArray data = reply->readAll();
    const QJsonDocument doc = QJsonDocument::fromJson(data);
    const QJsonObject object = doc.object();

    if (httpCode != 200) {
        const Error error = Error::fromJson(object);
        if (error.isValid()) {
            setError(error.message);
            return;
        }
        setError("Authentication failed");
        return;
    }

    const QJsonObject userObject = object["user"].toObject();
    m_userInfo = UserInfo::fromJson(userObject);
    emit authenticated(m_userInfo);
}

void TimeFlipApiClient::handleError(QNetworkReply *reply)
{
    if (reply->error() == QNetworkReply::NoError) {
        qDebug() << "No error";
        return;
    }

    const int httpCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    const QByteArray data = reply->readAll();
    qDebug() << data;
    const QJsonDocument doc = QJsonDocument::fromJson(data);
    const QJsonObject object = doc.object();

    if (httpCode != 200) {
        const Error error = Error::fromJson(object);
        if (error.isValid()) {
            setError(error.message);
            return;
        }
    }
    setError("Authentication failed");
}

void TimeFlipApiClient::setError(const QString &message)
{
    qDebug() << "Error:" << message;
    m_lastError = message;
    emit error(message);
}

} // namespace TimeFlipApi
