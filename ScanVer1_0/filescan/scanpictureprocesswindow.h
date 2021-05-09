#ifndef SCANPICTUREPROCESSWINDOW_H
#define SCANPICTUREPROCESSWINDOW_H

#include "src/ui/business/imageprocess/pictureprocesswindow.h"
#include <QImage>
#include <QWidget>

namespace Ui {
class ScanPictureProcessWindow;
}

class ScanPictureProcessWindow : public QWidget {
    Q_OBJECT

signals:
    void si_on_u_pushButton_scan_clicked();

public:
    explicit ScanPictureProcessWindow(QWidget* parent = nullptr);
    ~ScanPictureProcessWindow();

private slots:

    void on_u_pushButton_scan_clicked();

    void on_btnUndo_clicked();

    void on_btnRedo_clicked();

    void on_btnZoomIn_clicked();

    void on_btnZoomOut_clicked();

    void on_btnOriginal_clicked();

    void on_btnFitWidth_clicked();

    void on_btnFitHeight_clicked();

    void on_btnRotate_90_clicked();

    void on_btnRotate90_clicked();

    void on_btnRotate180_clicked();

    void on_btnClearSelect_clicked();

    void on_btnCutBorder_clicked();

    void on_btnClearDeselect_clicked();

    void on_btnUpdate_clicked();

    void on_btnDrag_clicked();

    void on_btnAutoChangeAngle_clicked();

    void on_btnChangeAngle_clicked();

    void on_btnSet_clicked();

private:
    Ui::ScanPictureProcessWindow* ui;

public:
    PictureProcessWindow* PicWin;
    void InitJPG(QString allfilepath);
    void CanScan(bool enable);
};

#endif // SCANPICTUREPROCESSWINDOW_H
