#pragma once

#include <QDialog>

namespace Ui {
class Summary;
}

class Summary : public QDialog
{
    Q_OBJECT

public:
    explicit Summary(QWidget *parent = nullptr);
    ~Summary();

public slots:
    void updateActiveFacet(const QString &name, const QString &color);

private:
    Ui::Summary *ui;
};
