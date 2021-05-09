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
    file=new FileDirectoryForm();
    addPage(ui->groupBox, file);
    scanner=new ScannerForm(ui->groupBox_Scanner);
    addPage(ui->groupBox_Scanner, scanner);
    handle=new HandleForm(ui->groupBox_Handle);
    addPage(ui->groupBox_Handle, handle);
    high=new HighForm(ui->groupBox_high);
    addPage(ui->groupBox_high, high);
    pdf=new PdfForm(ui->groupBox_Pdf);
    addPage(ui->groupBox_Pdf, pdf);
    ui->stackedWidget->setCurrentIndex(0);
}
void MainWindow::addPage(QWidget* page, QWidget* w)
{
    QHBoxLayout* v = new QHBoxLayout();
    page->setLayout(v);
    v->addWidget(w);
    v->setContentsMargins(0, 0, 0, 0);
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
    ui->stackedWidget->setCurrentIndex(3);
}
