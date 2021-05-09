#include "highform.h"
#include "ui_highform.h"

HighForm::HighForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::HighForm)
{
    ui->setupUi(this);
}

HighForm::~HighForm()
{
    delete ui;
}
