#ifndef SCANNERCONTINUEFORM_H
#define SCANNERCONTINUEFORM_H

#include <QApplication>
#include <QWidget>
#include <QObject>
#include <QTimer>
#include <QDialog>
#include <QDebug>
#include <QKeyEvent>
#include <QDesktopWidget>


namespace Ui {
class ScannerContinueForm;
}

class ScannerContinueForm : public QDialog
{
    Q_OBJECT
public:
    explicit ScannerContinueForm(QWidget *parent = nullptr,int itime=30);
    ~ScannerContinueForm();
    static bool DoShow(QWidget *parent = nullptr,int itime=30);
private slots:
    void on_pushButton_ok_clicked();
    void on_pushButton_cancel_clicked();
    void dotime();
    void keyReleaseEvent(QKeyEvent *event);
private:
    Ui::ScannerContinueForm *ui;
    QTimer m_qtimer;
    int m_inttime;
};

#endif // SCANNERCONTINUEFORM_H
