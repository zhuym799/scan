#include "hdpictureprocesswindow.h"
#include "ui_hdpictureprocesswindow.h"
HDPictureProcessWindow::HDPictureProcessWindow(QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::HDPictureProcessWindow)
{
    ui->setupUi(this);
    QRegExpValidator* v = new QRegExpValidator(this);
    v->setRegExp(QRegExp(R"(^([3-9]|[1-9]\d)$)"));
    ui->edtStampWith->setValidator(v);
    changeGammaWindow = nullptr;
    QVBoxLayout* u_QVBoxLayout_pic = new QVBoxLayout();
    u_QVBoxLayout_pic->setContentsMargins(0, 0, 0, 0);
    ui->u_widget_PictureProcessWindow->setLayout(u_QVBoxLayout_pic);
    PicWin = new PictureProcessWindow(ui->u_widget_PictureProcessWindow);
    u_QVBoxLayout_pic->addWidget(PicWin);
    ui->edtEraserWidth->setValidator(new QIntValidator(1, 100, this));
    ui->edtMixColorValue->setValidator(new QIntValidator(0, 255, this));
    ui->edtStainWidth->setValidator(new QIntValidator(1, 100, this));
    ui->chkCenter->setChecked(PicWin->imageView->getCenterState());
    showMoreMenu = new QMenu(this);
    QStringList menuNames;
    menuNames << "引用原始图像"
              << "放大图像"
              << "缩小图像"
              << "原始大小"
              << "等宽"
              << "等高";
    for (int i = 0; i < menuNames.count(); i++)
    {
        showMoreMenu->addAction(menuNames.at(i) + "   " + PicWin->imageView->getShortCutkey(menuNames.at(i)));
    }
    connect(showMoreMenu, SIGNAL(triggered(QAction*)), this,
            SLOT(onShowMoreMenutriggered(QAction*)));

    connect(PicWin->imageView, SIGNAL(onKeyPressEvent(QKeyEvent*)),
            this, SLOT(onKeyPressEvent(QKeyEvent*)));
    connect(PicWin->imageView, SIGNAL(onGetSelectGamma(int, int, int, int, Mat&)), this, SLOT(onGetSelectGamma(int, int, int, int, Mat&)));
    //设置按钮快捷键提示
    ui->btnUndo->setToolTip(ui->btnUndo->toolTip() + " " + PicWin->imageView->getShortCutKeyInfo("撤销"));
    ui->btnRedo->setToolTip(ui->btnRedo->toolTip() + " " + PicWin->imageView->getShortCutKeyInfo("重做"));
    ui->btnDrag->setToolTip(ui->btnDrag->toolTip() + " " + PicWin->imageView->getShortCutKeyInfo("拖拽"));
    ui->btnClearSel->setToolTip(ui->btnClearSel->toolTip() + " " + PicWin->imageView->getShortCutKeyInfo("清除所选区域"));
    ui->btnClearDeSelect->setToolTip(ui->btnClearDeSelect->toolTip() + " " + PicWin->imageView->getShortCutKeyInfo("清除非选择区域"));
    ui->btnFillHole->setToolTip(ui->btnFillHole->toolTip() + " " + PicWin->imageView->getShortCutKeyInfo("去孔洞"));
    ui->btnErase->setToolTip(ui->btnErase->toolTip() + " " + PicWin->imageView->getShortCutKeyInfo("橡皮擦"));
    ui->btnChangeMixColor2->setToolTip(ui->btnChangeMixColor2->toolTip() + " " + PicWin->imageView->getShortCutKeyInfo("清除选定杂点"));
    ui->btnRemoveStains->setToolTip(ui->btnRemoveStains->toolTip() + " " + PicWin->imageView->getShortCutKeyInfo("局部去污"));
    ui->btnSaveFile->setToolTip(ui->btnSaveFile->toolTip() + " " + PicWin->imageView->getShortCutKeyInfo("保存"));
    ui->btnGetColor->setToolTip(ui->btnGetColor->toolTip() + " " + PicWin->imageView->getShortCutKeyInfo("获取颜色"));
    QString value;
    value = PicWin->imageView->getConfigValue("eraserwidth", "50");
    ui->edtEraserWidth->setText(value);
    value = PicWin->imageView->getConfigValue("mixcolorvalue", "30");
    ui->edtMixColorValue->setText(value);
    value = PicWin->imageView->getConfigValue("stainwidth", "7");
    ui->edtStainWidth->setText(value);
    value = PicWin->imageView->getConfigValue("stampwidth", "7");
    ui->edtStampWith->setText(value);
    connect(PicWin->imageView, SIGNAL(focus2xpc()), this, SLOT(focus2xpc()));
    connect(PicWin->imageView, SIGNAL(focus2zs()), this, SLOT(focus2zs()));
    connect(PicWin->imageView, SIGNAL(focus2wd()), this, SLOT(focus2wd()));

}

HDPictureProcessWindow::~HDPictureProcessWindow()
{
    PicWin->imageView->setConfigValue("eraserwidth", ui->edtEraserWidth->text());
    PicWin->imageView->setConfigValue("mixcolorvalue", ui->edtMixColorValue->text());
    PicWin->imageView->setConfigValue("stainwidth", ui->edtStainWidth->text());
    PicWin->imageView->setConfigValue("stampwidth", ui->edtStampWith->text());
    delete ui;
}

void HDPictureProcessWindow::focus2xpc()
{
    ui->edtEraserWidth->setFocus();
}
void HDPictureProcessWindow::focus2zs()
{
    ui->edtMixColorValue->setFocus();
}
void HDPictureProcessWindow::focus2wd()
{
    ui->edtStainWidth->setFocus();
}

void HDPictureProcessWindow::InitJPG(QString allfilepath)
{
    PicWin->InitPic(allfilepath);
}

void HDPictureProcessWindow::useOrigialImage()
{
    Mat mat = OrgWindow->PicWin->imageView->getMat();
    PicWin->imageView->saveImgDataUndoList();
    PicWin->imageView->copyMat(mat);
    PicWin->imageView->saveFile();
}

void HDPictureProcessWindow::on_btnErase_clicked()
{
    PicWin->imageView->changeOperateType(ImageView::imErase);
    PicWin->imageView->setEraserWidth(ui->edtEraserWidth->text().toInt());
}

void HDPictureProcessWindow::on_edtEraserWidth_textChanged(const QString& arg1)
{
    Q_UNUSED(arg1)
    PicWin->imageView->setEraserWidth(ui->edtEraserWidth->text().toInt());
}

void HDPictureProcessWindow::on_btnShowMore_clicked()
{
    QPoint pt;
    pt.setX(0);
    pt.setY(ui->btnShowMore->height());
    pt = ui->btnShowMore->mapToGlobal(pt);
    showMoreMenu->popup(pt);
}

void HDPictureProcessWindow::onShowMoreMenutriggered(QAction* action)
{
    QString text = action->text();
    if (text.indexOf("放大图像") == 0)
    {
        PicWin->imageView->zoomIn();
    }
    else if (text.indexOf("缩小图像") == 0)
    {
        PicWin->imageView->zoomOut();

    }
    else if (text.indexOf("原始大小") == 0)
    {
        PicWin->imageView->showIn100PercentView();
    }
    else if (text.indexOf("等高") == 0)
    {
        PicWin->imageView->showFitInHeight();

    }
    else if (text.indexOf("等宽") == 0)
    {
        PicWin->imageView->showFitInWidth();
    }
    else if (text.indexOf("引用原始图像") == 0)
    {
        useOrigialImage();
    }
}

void HDPictureProcessWindow::on_btnClearSel_clicked()
{
    PicWin->imageView->changeOperateType(ImageView::imClearSelect);
}

void HDPictureProcessWindow::on_btnClearDeSelect_clicked()
{
    PicWin->imageView->changeOperateType(ImageView::imClearDeSelect);
}

void HDPictureProcessWindow::on_btnFillHole_clicked()
{
    PicWin->imageView->changeOperateType(ImageView::imFillHole);
}

void HDPictureProcessWindow::on_btnChangeMixColor_clicked()
{
    PicWin->imageView->changeOperateType(ImageView::imRemoveMixColor);
    PicWin->imageView->changeMixColorValue(ui->edtMixColorValue->text().toInt());
}

void HDPictureProcessWindow::on_btnChangeMixColor2_clicked()
{
    PicWin->imageView->changeOperateType(ImageView::imRemoveMixColor2);
    PicWin->imageView->changeMixColorValue(ui->edtMixColorValue->text().toInt());
}

void HDPictureProcessWindow::on_btnUndo_clicked()
{
    PicWin->imageView->undo();
}

void HDPictureProcessWindow::on_btnRedo_clicked()
{
    PicWin->imageView->redo();
}

void HDPictureProcessWindow::on_btnSaveFile_clicked()
{
    PicWin->imageView->saveFile();
}

void HDPictureProcessWindow::on_btnRemoveStains_clicked()
{
    ui->btnRemoveStains->setEnabled(false);
    QApplication::processEvents();
    PicWin->imageView->RemoveStains();
    ui->btnRemoveStains->setEnabled(true);
}

void HDPictureProcessWindow::on_toolButton_clicked()
{
    PicWin->imageView->changeOperateType(ImageView::imRemoveStains);
    PicWin->imageView->setStainWidth(ui->edtStainWidth->text().toInt());
}

void HDPictureProcessWindow::on_btnDrag_clicked()
{
    PicWin->imageView->changeOperateType(ImageView::imDrag);
}

void HDPictureProcessWindow::on_chkMarkRuler_clicked()
{
    Qt::CheckState b = ui->chkMarkRuler->checkState();
    if (b == Qt::Checked)
    {
        PicWin->setMarkRulerShow(true);

    }
    else
    {
        PicWin->setMarkRulerShow(false);
    }
}

void HDPictureProcessWindow::on_chkScrollBar_clicked()
{
    Qt::CheckState b = ui->chkScrollBar->checkState();
    if (b == Qt::Checked)
    {
        PicWin->setScrollBarShow(true);

    }
    else
    {
        PicWin->setScrollBarShow(false);
    }
}

void HDPictureProcessWindow::on_edtStainWidth_textChanged(const QString& arg1)
{
    Q_UNUSED(arg1)
    PicWin->imageView->setStainWidth(ui->edtStainWidth->text().toInt());
}

void HDPictureProcessWindow::onKeyPressEvent(QKeyEvent* event)
{
    Q_UNUSED(event)
    useOrigialImage();
}

void HDPictureProcessWindow::on_btnGamma_clicked()
{
    PicWin->imageView->changeOperateType(ImageView::imGamma);
}

void HDPictureProcessWindow::onGetSelectGamma(int x, int y, int w, int h, Mat& mat)
{
    if (changeGammaWindow != nullptr)
    {
        delete changeGammaWindow;
    }
    changeGammaWindow = new ChangeGammaWindow(this);
    connect(changeGammaWindow, SIGNAL(onChangeGamma(int, int, int, int, Mat&)), this, SLOT(onChangeGamma(int, int, int, int, Mat&)));
    changeGammaWindow->show();
    changeGammaWindow->x = x;
    changeGammaWindow->y = y;
    changeGammaWindow->w = w;
    changeGammaWindow->h = h;
    changeGammaWindow->UpdateUI();
    changeGammaWindow->gammaImage->loadMat(mat);
}

void HDPictureProcessWindow::onChangeGamma(int x, int y, int w, int h, Mat& mat)
{
    PicWin->imageView->ReplaceMat(x, y, w, h, mat);
}

void HDPictureProcessWindow::on_chkCenter_stateChanged(int arg1)
{
    Q_UNUSED(arg1)
    PicWin->imageView->setCenterState(ui->chkCenter->isChecked());
}

void HDPictureProcessWindow::on_btnGetColor_clicked()
{
    PicWin->imageView->changeOperateType(ImageView::imGetColor);
    PicWin->imageView->changeMixColorValue(ui->edtMixColorValue->text().toInt());
}

void HDPictureProcessWindow::on_btnStamp_clicked()
{
    PicWin->imageView->changeOperateType(ImageView::imFixStamp);
}

void HDPictureProcessWindow::on_edtStampWith_textChanged(const QString& arg1)
{
    Q_UNUSED(arg1)
    int stampWidth = ui->edtStampWith->text().toInt();
    PicWin->imageView->setStampWidth(stampWidth);
}
