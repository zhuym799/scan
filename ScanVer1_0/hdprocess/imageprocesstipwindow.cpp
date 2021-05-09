#include "imageprocesstipwindow.h"
#include "ui_imageprocesstipwindow.h"

ImageProcessTipWindow::ImageProcessTipWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::ImageProcessTipWindow)
{
    ui->setupUi(this);
}

ImageProcessTipWindow::~ImageProcessTipWindow()
{
    delete ui;
}
