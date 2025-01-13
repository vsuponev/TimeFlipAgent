#include "Summary.h"
#include "ui_Summary.h"

Summary::Summary(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::Summary)
{
    ui->setupUi(this);
}

Summary::~Summary()
{
    delete ui;
}

void Summary::updateActiveFacet(const QString &name, const QString &color)
{
    const QString taskDescription = QStringLiteral("<span style='font-weight:bold;color:%1;'>%2</span>").arg(color, name);
    ui->currentTask->setText(taskDescription);
}
