#include "Configuration.h"
#include "ui_Configuration.h"

#include "Config.h"

#include <QDesktopServices>

Configuration::Configuration(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::Configuration)
{
    ui->setupUi(this);

    connect(ui->saveButton, &QPushButton::clicked, this, &Configuration::onSaveTriggered);
    connect(ui->cancelButton, &QPushButton::clicked, this, &QDialog::close);
    connect(ui->signupLabel, &QLabel::linkActivated, [](const QString &link) {
        QDesktopServices::openUrl(link);
    });
}

Configuration::~Configuration()
{
    delete ui;
}

void Configuration::showEvent(QShowEvent *event)
{
    QDialog::showEvent(event);

    initConfiguration();
}

void Configuration::initConfiguration()
{
    const Config &config = Config::instance();
    ui->usernameEdit->setText(config.email);
    ui->passwordEdit->setText(config.password);
}

void Configuration::onSaveTriggered()
{
    Config &config = Config::instance();

    const QString newUsername = ui->usernameEdit->text();
    const QString newPassword = ui->passwordEdit->text();

    if (newUsername == config.email && newPassword == config.password) {
        close();
        return;
    }

    // Something was updated, save the config
    config.email = ui->usernameEdit->text();
    config.password = ui->passwordEdit->text();
    config.save();
    emit configurationUpdated();

    close();
}
