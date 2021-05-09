#include "filedirectoryform.h"
#include "ui_filedirectoryform.h"

FileDirectoryForm::FileDirectoryForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FileDirectoryForm)
{
    ui->setupUi(this);
}

FileDirectoryForm::~FileDirectoryForm()
{
    delete ui;
}
