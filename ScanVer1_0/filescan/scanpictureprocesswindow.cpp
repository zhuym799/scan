#include "scanpictureprocesswindow.h"
#include "ui_scanpictureprocesswindow.h"

ScanPictureProcessWindow::ScanPictureProcessWindow(QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::ScanPictureProcessWindow)
{
    ui->setupUi(this);
    QVBoxLayout* u_QVBoxLayout_pic = new QVBoxLayout();
    u_QVBoxLayout_pic->setContentsMargins(0, 0, 0, 0);
    ui->u_widget_PictureProcessWindow->setLayout(u_QVBoxLayout_pic);
    PicWin = new PictureProcessWindow(ui->u_widget_PictureProcessWindow);
    u_QVBoxLayout_pic->addWidget(PicWin);
    PicWin->imageView->setImageViewType(ImageView::imTypeOriginal);
}

ScanPictureProcessWindow::~ScanPictureProcessWindow()
{
    delete ui;
}

//扫描
void ScanPictureProcessWindow::on_u_pushButton_scan_clicked()
{
    emit si_on_u_pushButton_scan_clicked();
}

void ScanPictureProcessWindow::InitJPG(QString allfilepath)
{
    PicWin->InitPic(allfilepath);
    PicWin->StretchHeight();
}

void ScanPictureProcessWindow::CanScan(bool enable)
{
    ui->u_pushButton_scan->setEnabled(enable);
}
void ScanPictureProcessWindow::on_btnUndo_clicked()
{
    PicWin->imageView->undo();
}

void ScanPictureProcessWindow::on_btnRedo_clicked()
{
    PicWin->imageView->redo();
}

void ScanPictureProcessWindow::on_btnZoomIn_clicked()
{
    PicWin->imageView->zoomIn();
}

void ScanPictureProcessWindow::on_btnZoomOut_clicked()
{
    PicWin->imageView->zoomOut();
}

void ScanPictureProcessWindow::on_btnOriginal_clicked()
{
    PicWin->imageView->showIn100PercentView();
}

void ScanPictureProcessWindow::on_btnFitWidth_clicked()
{
    PicWin->imageView->showFitInWidth();
}

void ScanPictureProcessWindow::on_btnFitHeight_clicked()
{
    PicWin->imageView->showFitInHeight();
}

void ScanPictureProcessWindow::on_btnRotate_90_clicked()
{
    PicWin->imageView->flipImage(-90);
}

void ScanPictureProcessWindow::on_btnRotate90_clicked()
{
    PicWin->imageView->flipImage(90);
}

void ScanPictureProcessWindow::on_btnRotate180_clicked()
{
    PicWin->imageView->flipImage(180);
}

void ScanPictureProcessWindow::on_btnClearSelect_clicked()
{
    PicWin->imageView->changeOperateType(ImageView::imClearSelectOriginal);
}

void ScanPictureProcessWindow::on_btnCutBorder_clicked()
{
    PicWin->imageView->changeOperateType(ImageView::imCutBorder);
}

void ScanPictureProcessWindow::on_btnClearDeselect_clicked()
{
    PicWin->imageView->changeOperateType(ImageView::imClearDeSelectOriginal);
}

void ScanPictureProcessWindow::on_btnUpdate_clicked()
{
    PicWin->imageView->update();
}

void ScanPictureProcessWindow::on_btnDrag_clicked()
{
    PicWin->imageView->changeOperateType(ImageView::imDrag);
}

void ScanPictureProcessWindow::on_btnAutoChangeAngle_clicked()
{
    PicWin->imageView->autoCorrectImageAngle();
    PicWin->imageView->saveFile();
}

void ScanPictureProcessWindow::on_btnChangeAngle_clicked()
{
    PicWin->imageView->changeOperateType(ImageView::imRotate);
}

void ScanPictureProcessWindow::on_btnSet_clicked()
{
    PicWin->imageView->SetShortCutWindow();
}
