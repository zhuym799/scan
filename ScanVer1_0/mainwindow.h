#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
const QString softwares="文书扫描管理系统";
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
};

#endif // MAINWINDOW_H
