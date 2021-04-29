#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowState(Qt::WindowMaximized);
    this->setWindowModality(Qt::ApplicationModal);
    setWindowTitle("");
    setWindowIcon(QIcon(""));
}

MainWindow::~MainWindow()
{
    delete ui;
}
