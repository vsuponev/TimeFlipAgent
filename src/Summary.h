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

private:
    Ui::Summary *ui;
};
