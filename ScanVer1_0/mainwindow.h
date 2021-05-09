#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QMainWindow>
#include "scannerform.h"
#include "handleform.h"
#include "highform.h"
#include "pdfform.h"
#include "filedirectoryform.h"
namespace Ui {
class MainWindow;
}
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_toolButton_scan_clicked();

    void on_toolButton_handle_clicked();

    void on_toolButton_high_clicked();

    void on_toolButton_pdf_clicked();

private:
    Ui::MainWindow *ui;
    //文件目录
    FileDirectoryForm *file;
    //扫描
    ScannerForm *scanner;
    //处理
    HandleForm *handle;
    //图像转高清
    HighForm *high;
    //图像合成FDF
    PdfForm *pdf;

    void addPage(QWidget* page, QWidget* w);
};

#endif // MAINWINDOW_H
