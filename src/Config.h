#pragma once

#include <QString>

class Config
{
public:
    Config();

    static Config &instance();

    bool isValid() const;

    QString configFilePath() const;

    bool load();
    bool save();

public:
    QString email;
    QString password;
};
