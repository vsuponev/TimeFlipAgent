#include "Config.h"

#include <QApplication>
#include <QDebug>
#include <QDir>
#include <QSettings>
#include <QStandardPaths>

Config::Config() {}

Config &Config::instance()
{
    static Config instance;
    return instance;
}

bool Config::isValid() const
{
    return !username.isEmpty() && !password.isEmpty();
}

QString Config::configFilePath() const
{
    QString configDir = QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation);
    if (configDir.isEmpty()) {
        // Fallback to home directory if AppConfigLocation is not available
        configDir = QDir::homePath() + "/.config/" + QApplication::applicationName();
    }

    QDir dir(configDir);
    if (!dir.exists()) {
        dir.mkpath(".");
    }

    return dir.filePath("config.ini");
}

bool Config::load()
{
    const QString path = configFilePath();

    qDebug() << "Reading config file" << path;
    if (!QFileInfo::exists(path)) {
        qDebug() << "Config file doesn't exist!";
        return false;
    }

    QSettings settings(path, QSettings::IniFormat);

    settings.beginGroup("Credentials");
    username = settings.value("username", "").toString();
    password = settings.value("password", "").toString();
    settings.endGroup();

    return true;
}

bool Config::save()
{
    const QString path = configFilePath();
    qDebug() << "Saving to config file" << path;

    QSettings settings(path, QSettings::IniFormat);

    // Begin writing to the INI file
    settings.beginGroup("Credentials");
    settings.setValue("username", username);
    settings.setValue("password", password);
    settings.endGroup();

    settings.sync();

    return true;
}
