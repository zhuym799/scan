#include "scannerform.h"
#include "ui_scannerform.h"

ScannerForm::ScannerForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ScannerForm)
{
    ui->setupUi(this);
}

ScannerForm::~ScannerForm()
{
    delete ui;
}
