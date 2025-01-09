#pragma once

#include <QDialog>

namespace Ui {
class Configuration;
}

class Configuration : public QDialog
{
    Q_OBJECT

public:
    explicit Configuration(QWidget *parent = nullptr);
    ~Configuration();

signals:
    void configurationUpdated();

protected:
    void showEvent(QShowEvent *event) override;

private slots:
    void initConfiguration();
    void onSaveTriggered();

private:
    Ui::Configuration *ui;
};

