#include "handleform.h"
#include "ui_handleform.h"

HandleForm::HandleForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::HandleForm)
{
    ui->setupUi(this);
}

HandleForm::~HandleForm()
{
    delete ui;
}
