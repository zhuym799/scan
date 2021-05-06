#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "qscanglobal.h"
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowState(Qt::WindowMaximized);
    this->setWindowModality(Qt::ApplicationModal);
    QPalette pal;
    pal.setColor(QPalette::WindowText, Qt::white);
    ui->label->setPalette(pal);
    setWindowTitle(softwares);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_toolButton_scan_clicked()
{
    ui->stackedWidget->setCurrentIndex(0);
}

void MainWindow::on_toolButton_handle_clicked()
{
    ui->stackedWidget->setCurrentIndex(1);
}

void MainWindow::on_toolButton_high_clicked()
{
    ui->stackedWidget->setCurrentIndex(2);
}

void MainWindow::on_toolButton_pdf_clicked()
{

}
