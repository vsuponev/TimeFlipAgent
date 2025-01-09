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
