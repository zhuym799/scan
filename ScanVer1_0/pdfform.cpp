#include "pdfform.h"
#include "ui_pdfform.h"

PdfForm::PdfForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PdfForm)
{
    ui->setupUi(this);
}

PdfForm::~PdfForm()
{
    delete ui;
}
