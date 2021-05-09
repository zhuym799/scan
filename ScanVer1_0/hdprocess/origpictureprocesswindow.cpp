#include "origpictureprocesswindow.h"
#include "ui_origpictureprocesswindow.h"
#include <QMenu>
#include <QMessageBox>
#include <QRegExpValidator>
#include <QToolTip>
#include <QJsonObject>
#include <QBuffer>
//#include "opencv2/highgui/highgui.hpp"
//#include "opencv2/imgproc/imgproc.hpp"
#include "src/umsp-hfes-client/umsp.h"
#include "src/global/controlutils.h"
#include "src/common/filescontrol.h"
#include "imageprocesstipwindow.h"
#include "hdpictureprocesswindow.h"
#include "src/global/qarchscanglobal.h"
#include "src/ui/business/imageprocess/batchchangeanglewindow.h"
#include "src/ui/business/imageprocess/imageviewhistogramdialog.h"
#include "src/ui/business/imageprocess/pictureprocesswindow.h"
#include "src/ui/business/personinfo/personbaseinfo.h"
#include "src/control/imagecontrol/imageviewobject.h"

OrigPictureProcessWindow::OrigPictureProcessWindow(QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::OrigPictureProcessWindow)
{
    ui->setupUi(this);

#ifndef QT_AI
    ui->btnAICorrectAngle->setVisible(false);
    ui->btnAICorrectAngle->setEnabled(false);
    ui->btnAICutBorder1->setVisible(false);
    ui->btnAICutBorder1->setEnabled(false);
    ui->btnAICutBorder2->setVisible(false);
    ui->btnAICutBorder2->setEnabled(false);
    ui->btnAIRemoveStain->setVisible(false);
    ui->btnAIRemoveStain->setEnabled(false);
    ui->btnAIBatCutBorder1->setVisible(false);
    ui->btnAIBatCutBorder1->setEnabled(false);
    ui->btnAIBatCutBorder2->setVisible(false);
    ui->btnAIBatCutBorder2->setEnabled(false);
    ui->btnAIBatCorrectAngle->setVisible(false);
    ui->btnAIBatCorrectAngle->setEnabled(false);
    ui->btnAIBatRemoveStain->setVisible(false);
    ui->btnAIBatRemoveStain->setEnabled(false);
#endif
    HdWindow = nullptr;

    PicWin = new PictureProcessWindow(ui->u_widget_PictureProcessWindow);

    bool b = PicWin->imageView->IsLocked();
    ui->chkLocker->setChecked(b);
    ui->chkLocker->setCheckable(true);

    //显示弹出菜单
    showMoreMenu = new QMenu(this);
    QStringList MenuNames;
    MenuNames << "放大图像"
              << "缩小图像"
              << "原始大小"
              << "等宽"
              << "等高"
              << "顺时针旋转90度"
              << "逆时针旋转90度"
              << "旋转180度"
              << "抠取圆章"
              << "取红色"
              << "取蓝色"
              << "抠取表格线"
              << "计算灰度值"
              << "计算方差值";

    for (int i = 0; i < MenuNames.count(); i++)
        showMoreMenu->addAction(MenuNames.at(i) + "  " + PicWin->imageView->getShortCutkey(MenuNames.at(i)));
    connect(showMoreMenu, SIGNAL(triggered(QAction*)), this,
            SLOT(onShowMoreMenutriggered(QAction*)));
    connect(PicWin->imageView, SIGNAL(onGetSelectMat(int, int, int, int, Mat&)), this,
            SLOT(onMyGetSelectMat(int, int, int, int, Mat&)));
    batChangeWindow = nullptr;

    connect(PicWin->imageView, SIGNAL(onKeyPressEvent(QKeyEvent*)),
            this, SLOT(onKeyPressEvent(QKeyEvent*)));
    //抠取照片
    connect(PicWin->imageView, SIGNAL(onGetSelectMat2(int, int, int, int, Mat&)), this, SLOT(onGetSelectMat2(int, int, int, int, Mat&)));
    connect(PicWin->imageView, SIGNAL(onGetSelectMat3(int, int, int, int, Mat&)), this, SLOT(onGetSelectMat3(int, int, int, int, Mat&)));
    connect(PicWin->imageView, SIGNAL(onGetSelectRed(int, int, int, int, Mat&)), this, SLOT(onGetSelectRed(int, int, int, int, Mat&)));
    connect(PicWin->imageView, SIGNAL(onGetSelectBlue(int, int, int, int, Mat&)), this, SLOT(onGetSelectBlue(int, int, int, int, Mat&)));
    connect(PicWin->imageView, SIGNAL(onGetSelectBlack(int, int, int, int, Mat&)), this, SLOT(onGetSelectBlack(int, int, int, int, Mat&)));
    connect(PicWin->imageView, SIGNAL(onGetPartClearMat(int, int, int, int, Mat&)), this, SLOT(onGetPartClearMat(int, int, int, int, Mat&)));
    ui->edtThreshold->setText(QString("%1").arg(PicWin->imageView->getThresholdValue()));
    ui->chkAutoThresholdValue->setChecked(PicWin->imageView->isAutoThresholdValue());
    ui->edtGamma->setText(QString("%1").arg(PicWin->imageView->getGammaValue()));
    connect(ui->edtThreshold, SIGNAL(onMouseDoubleClick()), this, SLOT(on_edtThreshold_MouseDoubleClick()));
    resizeEvent(nullptr);
    QRegExpValidator* v = new QRegExpValidator(this);
    v->setRegExp(QRegExp(R"(^[0-9]$)"));
    ui->edtGamma->setValidator(v);

    //设置按钮快捷键提示
    ui->btnUndo->setToolTip(ui->btnUndo->toolTip() + " " + PicWin->imageView->getShortCutKeyInfo("撤销"));
    ui->btnRedo->setToolTip(ui->btnRedo->toolTip() + " " + PicWin->imageView->getShortCutKeyInfo("重做"));
    ui->btnDrag->setToolTip(ui->btnDrag->toolTip() + " " + PicWin->imageView->getShortCutKeyInfo("拖拽"));
    ui->btnMannualChangeAngle->setToolTip(ui->btnMannualChangeAngle->toolTip() + " " + PicWin->imageView->getShortCutKeyInfo("切边"));
    ui->btnCutBorder->setToolTip(ui->btnCutBorder->toolTip() + " " + PicWin->imageView->getShortCutKeyInfo("纠偏"));
    ui->btnAutoCorrectAngle->setToolTip(ui->btnAutoCorrectAngle->toolTip() + " " + PicWin->imageView->getShortCutKeyInfo("自动纠偏"));
    ui->btnPartClear->setToolTip(ui->btnPartClear->toolTip() + " " + PicWin->imageView->getShortCutKeyInfo("局部优化"));
    ui->btnClearImage->setToolTip(ui->btnClearImage->toolTip() + " " + PicWin->imageView->getShortCutKeyInfo("图像优化"));

    ui->btnSave->setToolTip(ui->btnSave->toolTip() + " " + PicWin->imageView->getShortCutKeyInfo("保存"));

    imageProcessTipWindow = nullptr;

    QString value;
    value = PicWin->imageView->getConfigValue("threshold", "238");
    ui->edtThreshold->setText(value);

    value = PicWin->imageView->getConfigValue("gamma", "5");
    ui->edtGamma->setText(value);
    connect(PicWin->imageView, SIGNAL(focus2yz()), this, SLOT(focus2yz()));
    connect(PicWin->imageView, SIGNAL(focus2Gamma()), this, SLOT(focus2Gamma()));

}

OrigPictureProcessWindow::~OrigPictureProcessWindow()
{
    PicWin->imageView->setConfigValue("threshold", ui->edtThreshold->text());
    PicWin->imageView->setConfigValue("gamma", ui->edtGamma->text());

    delete ui;
}

void OrigPictureProcessWindow::focus2yz()
{
    ui->edtThreshold->setFocus();
}

void OrigPictureProcessWindow::focus2Gamma()
{
    ui->edtGamma->setFocus();
}
void OrigPictureProcessWindow::InitJPG(QString allfilepath)
{
    PicWin->InitPic(allfilepath);
}

void OrigPictureProcessWindow::hidePickPhotoButton()
{
    ui->btnPickPhoto->setHidden(true);
    ui->btnClearImage->setHidden(true);
    ui->btnPartClear->setHidden(true);
    ui->grpGamma->setHidden(true);
    ui->btnAIRemoveStain->setHidden(true);
    ui->btnAIBatRemoveStain->setHidden(true);
    for (int i = showMoreMenu->actions().count() - 1; i >= 0; i--)
    {
        QAction* act = showMoreMenu->actions().at(i);
        if (0 == act->text().indexOf("抠取圆章") || 0 == act->text().indexOf("取红色")
            || 0 == act->text().indexOf("取蓝色") || 0 == act->text().indexOf("抠取表格线"))
        {
            showMoreMenu->removeAction(act);
            delete act;
            act = nullptr;
        }
    }
}

//转优化
void OrigPictureProcessWindow::on_btnClearImage_clicked()
{
    if (!HdWindow || !HdWindow->PicWin || !HdWindow->PicWin->imageView)
        return;
    QString hdfilepath = PicWin->imageView->filePath;
    hdfilepath.replace("/sm/", "/gq/").replace("/ys/", "/gq/");
    ui->btnClearImage->setEnabled(false);
    QApplication::processEvents();
    Mat retData;
    if (!PicWin->imageView->clearImage(retData))
    {
        ui->btnClearImage->setEnabled(true);
        return;
    }
    HdWindow->PicWin->imageView->reLoadImgdata(retData);
    HdWindow->PicWin->imageView->saveImgDataUndoList();
    HdWindow->PicWin->imageView->reLoadImgData();
    HdWindow->PicWin->imageView->saveFile(hdfilepath);
    ui->btnClearImage->setEnabled(true);
}

void OrigPictureProcessWindow::on_btnMannualChangeAngle_clicked()
{
    PicWin->imageView->changeOperateType(ImageView::imRotate);
}

//切边
void OrigPictureProcessWindow::on_btnCutBorder_clicked()
{
    PicWin->imageView->changeOperateType(ImageView::imCutBorder);
}

//拖拽
void OrigPictureProcessWindow::on_btnDrag_clicked()
{
    PicWin->imageView->changeOperateType(ImageView::imDrag);
}

void OrigPictureProcessWindow::on_btnShowMore_clicked()
{
    QPoint pt;
    pt.setX(0);
    pt.setY(ui->btnShowMore->height());
    pt = ui->btnShowMore->mapToGlobal(pt);
    showMoreMenu->exec(pt);
}

void OrigPictureProcessWindow::onShowMoreMenutriggered(QAction* action)
{
    QString text = action->text();
    if (text.indexOf("放大图像") == 0)
        PicWin->imageView->zoomIn();
    else if (text.indexOf("缩小图像") == 0)
        PicWin->imageView->zoomOut();
    else if (text.indexOf("原始大小") == 0)
        PicWin->imageView->showIn100PercentView();
    else if (text.indexOf("等高") == 0)
        PicWin->imageView->showFitInHeight();
    else if (text.indexOf("等宽") == 0)
        PicWin->imageView->showFitInWidth();
    else if (text.indexOf("顺时针旋转90度") == 0)
        PicWin->imageView->flipImage(90);
    else if (text.indexOf("逆时针旋转90度") == 0)
        PicWin->imageView->flipImage(-90);
    else if (text.indexOf("旋转180度") == 0)
        PicWin->imageView->flipImage(180);
    else if (text.indexOf("抠取圆章") == 0)
    {
        if (HdWindow != nullptr)
            PicWin->imageView->changeOperateType(ImageView::imPickRoundStamp);
    }
    else if (text.indexOf("取红色") == 0)
    {
        if (HdWindow != nullptr)
            PicWin->imageView->changeOperateType(ImageView::imPickRed);
    }
    else if (text.indexOf("取蓝色") == 0)
    {
        if (HdWindow != nullptr)
            PicWin->imageView->changeOperateType(ImageView::imPickBlue);
    }
    else if (text.indexOf("抠取表格线") == 0)
    {
        if (HdWindow != nullptr)
            PicWin->imageView->changeOperateType(ImageView::imPickBlack);
    }
    else if (text.indexOf("计算灰度值") == 0)
        PicWin->imageView->calGrayValue(PicWin->imageView->imgData);
    else if (text.indexOf("计算方差值") == 0)
        PicWin->imageView->calVarianceValue(PicWin->imageView->imgData);
}

void OrigPictureProcessWindow::on_btnUndo_clicked()
{
    PicWin->imageView->undo();
}

void OrigPictureProcessWindow::on_btnRedo_clicked()
{
    PicWin->imageView->redo();
}

void OrigPictureProcessWindow::onMyGetSelectMat(int x, int y, int w, int h, Mat& mat)
{
    ImageView* destView = HdWindow->PicWin->imageView;
    destView->pasteMat(x, y, w, h, mat);
}

void OrigPictureProcessWindow::on_btnAutoCorrectAngle_clicked()
{
    PicWin->imageView->autoCorrectImageAngle();
    PicWin->imageView->saveFile();
    QToolTip::showText(this->cursor().pos(), "自动纠偏已完成。");
}

void OrigPictureProcessWindow::on_btnPickPhoto_clicked()
{
    PicWin->imageView->changeOperateType(ImageView::imPickOutPhoto);
}

void OrigPictureProcessWindow::on_btnBatchAutoChangAngle_clicked()
{
    if (batChangeWindow != nullptr)
    {
        delete batChangeWindow;
        batChangeWindow = nullptr;
    }
    //批量纠偏窗口
    batChangeWindow = new BatchChangeAngleWindow(this);
    connect(batChangeWindow, &BatchChangeAngleWindow::sigBatchChangeAngleWindowClose,
            this, [ = ]()
    {
        PicWin->imageView->reloadFile();
    });
    //获取高清图片路径
    QStringList srcFiles;
    QJsonObject rjsonobj;
    ControlUtils::getGqRelativeFiles(rjsonobj, m_A0100);
    QList<QVariant> list = rjsonobj.value("data").toVariant().toList();
    for (int i = 0; i < list.count(); ++i)
    {
        QVariantMap msp = list[i].toMap();
        srcFiles << msp["d01zb03"].toString().replace("sm", "ys") + "/" + msp["e01zb03"].toString();
    }
    batChangeWindow->srcFiles = srcFiles;
    batChangeWindow->show();
}

void OrigPictureProcessWindow::resizeEvent(QResizeEvent* event)
{
    QWidget::resizeEvent(event);
    PicWin->setGeometry(0, 0, ui->u_widget_PictureProcessWindow->width(),
                        ui->u_widget_PictureProcessWindow->height());
}

void OrigPictureProcessWindow::on_chkShowRuler_clicked()
{
    PicWin->setMarkRulerShow(ui->chkShowRuler->isChecked());
}

void OrigPictureProcessWindow::on_chkScrollBarShow_clicked()
{
    PicWin->setScrollBarShow(ui->chkScrollBarShow->isChecked());
}

void OrigPictureProcessWindow::on_btnSave_clicked()
{
    PicWin->imageView->saveFile();
}

void OrigPictureProcessWindow::onKeyPressEvent(QKeyEvent* event)
{
    Q_UNUSED(event)
    on_btnClearImage_clicked();
}

void OrigPictureProcessWindow::onGetSelectMat2(int x, int y, int w, int h, Mat& mat)
{
    Q_UNUSED(x)
    Q_UNUSED(y)
    Q_UNUSED(w)
    Q_UNUSED(h)
    QString path = QApplication::applicationDirPath() + "/tmp.jpg";
    Mat mat2;
    cvtColor(mat, mat2, COLOR_RGB2BGR);
    ::imwrite(path.toLocal8Bit().data(), mat2);
    if (QArchScanGlobal::question(this, "提示", "您要将选择的区域作为照片添加到基本信息吗？") == QMessageBox::No)
        return;

    QImage tmpImage(path);
    QByteArray bytes;
    QBuffer buffer(&bytes);
    tmpImage.save(&buffer, "jpg");
    QString base64str = QString("data:image/jpg;base64," + bytes.toBase64());
    buffer.close();

//    QString strMd5 = FilesControl::CopyFileToCache(path);

//    Umsp umsp;
//    if (!umsp.u_upload(strMd5))
//    {
//        //失败
//        QArchScanGlobal::information(this, "提示", "上传图像失败");
//    }
//    else
//    {
    PersonBaseInfo::savePhoto(m_A0100, base64str);
    QArchScanGlobal::information(this, "提示", "刷新人员后显示效果。");
    //}
}

void OrigPictureProcessWindow::onGetPartClearMat(int x, int y, int w, int h, Mat& mat)
{
    if (!HdWindow || !HdWindow->PicWin || !HdWindow->PicWin->imageView)
        return;
    if (HdWindow->PicWin->imageView->imgData.empty())
        return;
    HdWindow->PicWin->imageView->saveImgDataUndoList();
    Mat retData;
    PicWin->imageView->clearImage(mat, retData);
    HdWindow->PicWin->imageView->copyMatInRect(retData, x, y, w, h);
    HdWindow->PicWin->imageView->saveFile();
}

void OrigPictureProcessWindow::onGetSelectMat3(int x, int y, int w, int h, Mat& mat)
{
    HdWindow->PicWin->imageView->setGammaValue(ui->edtGamma->text().toFloat());
    HdWindow->PicWin->imageView->CopyRoundStamp(x, y, w, h, mat);
}

void OrigPictureProcessWindow::onGetSelectRed(int x, int y, int w, int h, Mat& mat)
{
    HdWindow->PicWin->imageView->CopyRed(x, y, w, h, mat);
}

void OrigPictureProcessWindow::onGetSelectBlue(int x, int y, int w, int h, Mat& mat)
{
    HdWindow->PicWin->imageView->CopyBlue(x, y, w, h, mat);
}

void OrigPictureProcessWindow::onGetSelectBlack(int x, int y, int w, int h, Mat& mat)
{
    HdWindow->PicWin->imageView->CopyBlack(x, y, w, h, mat);
}

void OrigPictureProcessWindow::on_btnPickPhoto2_clicked()
{
    PicWin->imageView->changeOperateType(ImageView::imPickOutPhoto2);
}

void OrigPictureProcessWindow::on_chkLocker_clicked()
{
    bool b = ui->chkLocker->isChecked();
    PicWin->imageView->SetLocked(b);
}

void OrigPictureProcessWindow::on_btnSet_clicked()
{
    PicWin->imageView->SetShortCutWindow();
    if (HdWindow != nullptr)
        HdWindow->PicWin->imageView->LoadShortCuts();
}

void OrigPictureProcessWindow::on_btnPartClear_clicked()
{
    PicWin->imageView->changeOperateType(ImageView::imPartClear);
}

void OrigPictureProcessWindow::on_edtThreshold_textChanged(const QString& arg1)
{
    Q_UNUSED(arg1)
    PicWin->imageView->setThresholdValue(ui->edtThreshold->text().toInt());
    ImageViewObject::thresholdValue = ui->edtThreshold->text().toInt();
}

void OrigPictureProcessWindow::on_chkAutoThresholdValue_stateChanged(int arg1)
{
    Q_UNUSED(arg1)
    PicWin->imageView->setAutoThresholdValue(ui->chkAutoThresholdValue->isChecked());
    ImageViewObject::autoThresholdValue = ui->chkAutoThresholdValue->isChecked();
}

void OrigPictureProcessWindow::on_edtGamma_textChanged(const QString& arg1)
{
    Q_UNUSED(arg1)
    PicWin->imageView->setGammaValue(ui->edtGamma->text().toFloat());
    ImageViewObject::gammaValue = ui->edtGamma->text().toFloat();
}

void OrigPictureProcessWindow::on_edtThreshold_MouseDoubleClick()
{
    if (PicWin->imageView->imgData.empty())
        return;
    ImageViewHistogramDialog* dlg = new ImageViewHistogramDialog(this);
    dlg->getMat(PicWin->imageView->imgData);
    int ret = dlg->exec();
    if (ret == QDialog::Accepted)
        ui->edtThreshold->setText(QString("%1").arg(dlg->imageViewHistogram->selValue));
    delete dlg;
}

void OrigPictureProcessWindow::on_btnTip_clicked()
{
    if (imageProcessTipWindow != nullptr)
        delete imageProcessTipWindow;
    imageProcessTipWindow = new ImageProcessTipWindow(this);
    imageProcessTipWindow->showNormal();
}

//AI纠偏
void OrigPictureProcessWindow::on_btnAICorrectAngle_clicked()
{
    if (!GlobalConfig::instance.getAIImageProcess_CorrectionAngle())
    {
        QArchScanGlobal::information(this, "提示", "未开启AI纠偏功能！");
        return;
    }
#ifdef QT_USE_UMSP_CryptCore
    CryptCore* m_cryptcore = CryptCore::GetCryptCore();
#endif
    QString sourcepath = PicWin->imageView->filePath;
    QString unCryptCore_sourcepath = FilesControl::GetTmpFileDir() + "/" + QArchScanGlobal::createGuid() + ".jpg";
    QString unCryptCore_destpath = FilesControl::GetTmpFileDir() + "/" + QArchScanGlobal::createGuid() + ".jpg";
    QString destpath = PicWin->imageView->filePath;

#ifdef QT_USE_UMSP_CryptCore
    //临时解密用完删除
    m_cryptcore->u_decryptFile(sourcepath, unCryptCore_sourcepath);
    QJsonObject lqjo;
    lqjo.insert("source", unCryptCore_sourcepath);
    lqjo.insert("dest", unCryptCore_destpath);
#else
    QJsonObject lqjo;
    lqjo.insert("source", sourcepath);
    lqjo.insert("dest", destpath);
#endif
    QJsonObject rqjo;
    if (!ControlUtils::ai_CorrectionAngle(rqjo, lqjo))
    {
        ControlUtils::ai_CorrectionAngle(rqjo, lqjo, true);
    }
#ifdef QT_USE_UMSP_CryptCore
    //临时加密
    m_cryptcore->u_encryptFile(unCryptCore_destpath, destpath);
#endif
    PicWin->imageView->reloadFile();
}

//批量纠偏
void OrigPictureProcessWindow::on_btnAIBatCorrectAngle_clicked()
{
    if (!GlobalConfig::instance.getAIImageProcess_CorrectionAngle())
    {
        QArchScanGlobal::information(this, "提示", "未开启AI纠偏功能！");
        return;
    }
    QJsonObject rjsonobj;
    if (ControlUtils::getGqRelativeFiles(rjsonobj, m_A0100))
    {

        QStringList sourceAllFilePathList;
        foreach (const QJsonValue& onejo, rjsonobj.value("data").toArray())
        {
            QString d01zb03 = onejo.toObject().value("d01zb03").toString().replace("sm", "ys");
            QString e01zb03 = onejo.toObject().value("e01zb03").toString();
            sourceAllFilePathList << FilesControl::GetLocalFilesDir() + "/" + d01zb03 + "/" + e01zb03;
        }
        DialogBatchImageProcess* win = new DialogBatchImageProcess(DialogBatchImageProcessType::ai_correctionAngle, sourceAllFilePathList.count(), this);
        win->sourceAllFilePathList = sourceAllFilePathList;
        win->destAllFilePathList = sourceAllFilePathList;
        if (win->exec() == QDialog::Accepted)
        {
            PicWin->imageView->reloadFile();
        }
        return;
    }
}




//AI切边
void OrigPictureProcessWindow::on_btnAICutBorder1_clicked()
{
    if (!GlobalConfig::instance.getAIImageProcess_CutImage())
    {
        QArchScanGlobal::information(this, "提示", "未开启AI切边功能！");
        return;
    }
#ifdef QT_USE_UMSP_CryptCore
    CryptCore* m_cryptcore = CryptCore::GetCryptCore();
#endif
    QString sourcepath = PicWin->imageView->filePath;
    QString unCryptCore_sourcepath = FilesControl::GetTmpFileDir() + "/" + QArchScanGlobal::createGuid() + ".jpg";
    QString unCryptCore_destpath = FilesControl::GetTmpFileDir() + "/" + QArchScanGlobal::createGuid() + ".jpg";
    QString destpath = PicWin->imageView->filePath;

#ifdef QT_USE_UMSP_CryptCore
    //临时解密用完删除
    m_cryptcore->u_decryptFile(sourcepath, unCryptCore_sourcepath);
    QJsonObject lqjo;
    lqjo.insert("source", unCryptCore_sourcepath);
    lqjo.insert("dest", unCryptCore_destpath);
#else
    QJsonObject lqjo;
    lqjo.insert("source", sourcepath);
    lqjo.insert("dest", destpath);
#endif
    QJsonObject rqjo;
    lqjo.insert("type", "1");
    lqjo.insert("top", GlobalConfig::instance.getAIImageProcess_CutImage_top());
    lqjo.insert("bottom",  GlobalConfig::instance.getAIImageProcess_CutImage_bottom());
    lqjo.insert("left",  GlobalConfig::instance.getAIImageProcess_CutImage_left());
    lqjo.insert("right",  GlobalConfig::instance.getAIImageProcess_CutImage_right());
    if (!ControlUtils::ai_CutImage(rqjo, lqjo))
    {
        ControlUtils::ai_CutImage(rqjo, lqjo, true);
    }
#ifdef QT_USE_UMSP_CryptCore
    //临时加密
    m_cryptcore->u_encryptFile(unCryptCore_destpath, destpath);
#endif
    PicWin->imageView->reloadFile();
}

//批量切边
void OrigPictureProcessWindow::on_btnAIBatCutBorder1_clicked()
{
    if (!GlobalConfig::instance.getAIImageProcess_CutImage())
    {
        QArchScanGlobal::information(this, "提示", "未开启AI切边功能！");
        return;
    }

    QJsonObject rjsonobj;
    if (ControlUtils::getGqRelativeFiles(rjsonobj, m_A0100))
    {

        QStringList sourceAllFilePathList;
        foreach (const QJsonValue& onejo, rjsonobj.value("data").toArray())
        {
            QString d01zb03 = onejo.toObject().value("d01zb03").toString().replace("sm", "ys");
            QString e01zb03 = onejo.toObject().value("e01zb03").toString();
            sourceAllFilePathList << FilesControl::GetLocalFilesDir() + "/" + d01zb03 + "/" + e01zb03;
        }
        DialogBatchImageProcess* win = new DialogBatchImageProcess(DialogBatchImageProcessType::ai_cutImage_type1, sourceAllFilePathList.count(), this);
        win->sourceAllFilePathList = sourceAllFilePathList;
        win->destAllFilePathList = sourceAllFilePathList;
        if (win->exec() == QDialog::Accepted)
        {
            PicWin->imageView->reloadFile();
        }
        return;
    }
}

void OrigPictureProcessWindow::on_btnAIRemoveStain_clicked()
{
    if (!GlobalConfig::instance.getAIImageProcess_CutImage())
    {
        QArchScanGlobal::information(this, "提示", "未开启AI转高清功能！");
        return;
    }
#ifdef QT_USE_UMSP_CryptCore
    CryptCore* m_cryptcore = CryptCore::GetCryptCore();
#endif
    QString sourcepath = PicWin->imageView->filePath;
    QString unCryptCore_sourcepath = FilesControl::GetTmpFileDir() + "/" + QArchScanGlobal::createGuid() + ".jpg";
    QString unCryptCore_destpath = FilesControl::GetTmpFileDir() + "/" + QArchScanGlobal::createGuid() + ".jpg";
    QString destpath = sourcepath;
    destpath = destpath.replace("/ys/", "/gq/");
#ifdef QT_USE_UMSP_CryptCore
    //临时解密用完删除
    m_cryptcore->u_decryptFile(sourcepath, unCryptCore_sourcepath);
    QJsonObject lqjo;
    lqjo.insert("source", unCryptCore_sourcepath);
    lqjo.insert("dest", unCryptCore_destpath);
    lqjo.insert("size", "300");
#else
    QJsonObject lqjo;
    lqjo.insert("source", sourcepath);
    lqjo.insert("dest", destpath);
    lqjo.insert("size", "300");
#endif
    QJsonObject rqjo;
    if (!ControlUtils::ai_RemoveStain(rqjo, lqjo))
    {
        ControlUtils::ai_RemoveStain(rqjo, lqjo, true);
    }
#ifdef QT_USE_UMSP_CryptCore
    //临时加密
    m_cryptcore->u_encryptFile(unCryptCore_destpath, destpath);
#endif
    HdWindow->PicWin->imageView->SetFilePath(destpath);
    HdWindow->PicWin->imageView->update();
}

void OrigPictureProcessWindow::on_btnAIBatRemoveStain_clicked()
{
    if (!GlobalConfig::instance.getAIImageProcess_CutImage())
    {
        QArchScanGlobal::information(this, "提示", "未开启AI转高清！");
        return;
    }

    QJsonObject rjsonobj;
    if (ControlUtils::getGqRelativeFiles(rjsonobj, m_A0100))
    {

        QStringList sourceAllFilePathList;
        QStringList destAllFilePathList;
        foreach (const QJsonValue& onejo, rjsonobj.value("data").toArray())
        {
            QString d01zb03 = onejo.toObject().value("d01zb03").toString().replace("sm", "ys");//原ys路径
            QString e01zb03 = onejo.toObject().value("e01zb03").toString();
            sourceAllFilePathList << FilesControl::GetLocalFilesDir() + "/" + d01zb03 + "/" + e01zb03;
            destAllFilePathList << FilesControl::GetLocalFilesDir() + "/" + d01zb03.replace("ys", "gq").replace("sm", "gq") + "/" + e01zb03;
        }
        DialogBatchImageProcess* win = new DialogBatchImageProcess(DialogBatchImageProcessType::ai_removeStain, sourceAllFilePathList.count(), this);
        win->sourceAllFilePathList = sourceAllFilePathList;
        win->destAllFilePathList = destAllFilePathList;
        if (win->exec() == QDialog::Accepted)
        {
            QString sourcepath = PicWin->imageView->filePath;
            QString destpath = sourcepath;
            destpath = destpath.replace("/ys/", "/gq/");
            HdWindow->PicWin->imageView->SetFilePath(destpath);
            HdWindow->PicWin->imageView->update();
        }
        return;
    }
}

void OrigPictureProcessWindow::on_btnAICutBorder2_clicked()
{
    if (!GlobalConfig::instance.getAIImageProcess_CutImage())
    {
        QArchScanGlobal::information(this, "提示", "未开启AI切边功能！");
        return;
    }
#ifdef QT_USE_UMSP_CryptCore
    CryptCore* m_cryptcore = CryptCore::GetCryptCore();
#endif
    QString sourcepath = PicWin->imageView->filePath;
    QString unCryptCore_sourcepath = FilesControl::GetTmpFileDir() + "/" + QArchScanGlobal::createGuid() + ".jpg";
    QString unCryptCore_destpath = FilesControl::GetTmpFileDir() + "/" + QArchScanGlobal::createGuid() + ".jpg";
    QString destpath = PicWin->imageView->filePath;

#ifdef QT_USE_UMSP_CryptCore
    //临时解密用完删除
    m_cryptcore->u_decryptFile(sourcepath, unCryptCore_sourcepath);
    QJsonObject lqjo;
    lqjo.insert("source", unCryptCore_sourcepath);
    lqjo.insert("dest", unCryptCore_destpath);
#else
    QJsonObject lqjo;
    lqjo.insert("source", sourcepath);
    lqjo.insert("dest", destpath);
#endif
    QJsonObject rqjo;
    lqjo.insert("type", "2");
    lqjo.insert("top", GlobalConfig::instance.getAIImageProcess_CutImage_top());
    lqjo.insert("bottom",  GlobalConfig::instance.getAIImageProcess_CutImage_bottom());
    lqjo.insert("left",  GlobalConfig::instance.getAIImageProcess_CutImage_left());
    lqjo.insert("right",  GlobalConfig::instance.getAIImageProcess_CutImage_right());
    if (!ControlUtils::ai_CutImage(rqjo, lqjo))
    {
        ControlUtils::ai_CutImage(rqjo, lqjo, true);
    }
#ifdef QT_USE_UMSP_CryptCore
    //临时加密
    m_cryptcore->u_encryptFile(unCryptCore_destpath, destpath);
#endif
    PicWin->imageView->reloadFile();
}

void OrigPictureProcessWindow::on_btnAIBatCutBorder2_clicked()
{
    if (!GlobalConfig::instance.getAIImageProcess_CutImage())
    {
        QArchScanGlobal::information(this, "提示", "未开启AI切边功能！");
        return;
    }

    QJsonObject rjsonobj;
    if (ControlUtils::getGqRelativeFiles(rjsonobj, m_A0100))
    {
        QStringList sourceAllFilePathList;
        foreach (const QJsonValue& onejo, rjsonobj.value("data").toArray())
        {
            QString d01zb03 = onejo.toObject().value("d01zb03").toString().replace("sm", "ys");
            QString e01zb03 = onejo.toObject().value("e01zb03").toString();
            sourceAllFilePathList << FilesControl::GetLocalFilesDir() + "/" + d01zb03 + "/" + e01zb03;
        }
        DialogBatchImageProcess* win = new DialogBatchImageProcess(DialogBatchImageProcessType::ai_cutImage_type2, sourceAllFilePathList.count(), this);
        win->sourceAllFilePathList = sourceAllFilePathList;
        win->destAllFilePathList = sourceAllFilePathList;
        if (win->exec() == QDialog::Accepted)
        {
            PicWin->imageView->reloadFile();
        }
        return;
    }
}
