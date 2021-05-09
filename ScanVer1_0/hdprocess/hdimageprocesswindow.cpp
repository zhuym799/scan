#include "hdimageprocesswindow.h"
#include "ui_hdimageprocesswindow.h"
#include <QDesktopServices>
#include <QFile>
#include <QKeyEvent>
#include <QMessageBox>
#include <QSplitter>
#include <QStandardItem>
#include <QVBoxLayout>
#include <QJsonObject>
#include "src/common/filescontrol.h"
#include "src/global/controlutils.h"
#include "src/global/filesearcher.h"
#include "src/ui/business/personinfo/archinfoeditdialog.h"
#include "src/global/filedownloader.h"
#include "src/global/fileuploader.h"
#include "src/global/qarchscanglobal.h"
#include "src/global/skincontrolor.h"
#include "src/ui/business/common/downanduploadprogress.h"
#include "src/ui/business/common/searchpersoninfoexform.h"
#include "src/ui/business/imageprocess/batchangehdwindow.h"
#include "src/ui/business/hdprocess/hdpictureprocesswindow.h"
#include "src/ui/business/hdprocess/origpictureprocesswindow.h"
#include "src/ui/business/imageprocess/printimagesdialog.h"
#include "src/ui/business/personinfo/archinfohelper.h"

HDImageProcessWindow::HDImageProcessWindow(QWidget* parent)
    : QMainWindow(parent)
    , ui(new Ui::HDImageProcessWindow)
{
    ui->setupUi(this);
    this->setStyleSheet(SkinControlor::skinText);
    selectedCatalogInfo = new TSelectedCatalogInfo();
    QArchScanGlobal::initSelectedCatalogInfo(selectedCatalogInfo);
    archInfoHelper = new ArchInfoHelper(this);
    connect(archInfoHelper, SIGNAL(onLoadData(QString)), this, SLOT(onLoadData(QString)));
    qsimCatalog = new QStandardItemModel(ui->u_treeView_catalog);
    archInfoHelper->init(ui->u_treeView_catalog, qsimCatalog);
    ui->u_treeView_catalog->installEventFilter(this);
    //文件列表
    qsimFilesList = new QStandardItemModel(ui->u_tableView_filelist);
    ui->u_tableView_filelist->setModel(qsimFilesList);
    QString names[] = { "名称", "存在" };
    QArchScanGlobal::setModelHeaders(qsimFilesList, names, 2);
    QArchScanGlobal::setModelHeaderTextAlign(qsimFilesList, 2, ui->u_tableView_filelist);
    int colWiths[2] = { 100, 200 };
    QArchScanGlobal::setTableViewColWidths(ui->u_tableView_filelist, colWiths, 2);
    ui->u_tableView_filelist->installEventFilter(this);
    //原始图像区域
    QVBoxLayout* u_QVBoxLayout_ori = new QVBoxLayout();
    u_QVBoxLayout_ori->setContentsMargins(1, 1, 1, 0);
    ui->yuanshiPnlClient->setLayout(u_QVBoxLayout_ori);
    u_frame_orig = new OrigPictureProcessWindow(ui->yuanshiPnlClient);
    u_QVBoxLayout_ori->addWidget(u_frame_orig);
    //原始图像区域
    QVBoxLayout* u_QVBoxLayout_hd = new QVBoxLayout();
    u_QVBoxLayout_hd->setContentsMargins(1, 1, 1, 0);
    ui->GaoQingPnlClient->setLayout(u_QVBoxLayout_hd);
    u_frame_hd = new HDPictureProcessWindow(ui->GaoQingPnlClient);
    u_frame_orig->HdWindow = u_frame_hd;
    u_frame_hd->OrgWindow = u_frame_orig;
    u_QVBoxLayout_hd->addWidget(u_frame_hd);
    u_frame_orig->PicWin->imageView->setImageViewType(ImageView::imTypeOriginal);
    u_frame_hd->PicWin->imageView->setImageViewType(ImageView::imTypeHD);
    batChangeHdWindow = nullptr;
    connect(u_frame_orig->PicWin->imageView, &ImageView::onChangeOperateType, this,
            &HDImageProcessWindow::onSrcImageViewChangeOperateType);
    connect(u_frame_hd->PicWin->imageView, &ImageView::onChangeOperateType, this,
            &HDImageProcessWindow::onHdImageViewChangeOperateType);
    searchPersonInfoExForm = new SearchPersonInfoExForm(ui->u_widget_client);
    connect(searchPersonInfoExForm, SIGNAL(onFormResize()), this, SLOT(onResize()));
    spl1 = new QSplitter(ui->wgtImgClient);
    spl1->addWidget(ui->YuanShiPnl);
    spl1->addWidget(ui->GaoQingPnl);
    spl1->setStretchFactor(0, 1);
    spl1->setStretchFactor(1, 1);
    spl1->setOrientation(Qt::Horizontal);
    u_frame_orig->PicWin->imageView->installEventFilter(this);
    u_frame_hd->PicWin->imageView->installEventFilter(this);
    ui->btnPrePage->setToolTip("Shift+空格键");
    ui->btnNextPage->setToolTip("空格键");
    connect(searchPersonInfoExForm->searchPersonInfoForm, SIGNAL(onSelectPerson(QTableView*, QString, QString, QString)), this,
            SLOT(onSelectPerson(QTableView*, QString, QString, QString)));
}

HDImageProcessWindow::~HDImageProcessWindow()
{
    delete ui;
}

void HDImageProcessWindow::resizeEvent(QResizeEvent* event)
{
    Q_UNUSED(event)
    searchPersonInfoExForm->setGeometry(0, 0, searchPersonInfoExForm->getCurrentWidth(), ui->u_widget_client->height() - 2);
    ui->wgtFileList->setGeometry(searchPersonInfoExForm->getCurrentWidth(), 2, 382, ui->u_widget_client->height());
    ui->wgtImgClient->setGeometry(searchPersonInfoExForm->getCurrentWidth() + 382, 2,
                                  ui->u_widget_client->width() - 382 - searchPersonInfoExForm->getCurrentWidth(),
                                  ui->u_widget_client->height() - 2);
    spl1->setGeometry(0, 0,
                      ui->wgtImgClient->width(),
                      ui->wgtImgClient->height());
}

void HDImageProcessWindow::setLoadedFinish(bool bFinished)
{
    loadFinished = bFinished;
}

void HDImageProcessWindow::closeEvent(QCloseEvent* event)
{
    if (!loadFinished)
        event->ignore();
    else
        QMainWindow::closeEvent(event);
}

void HDImageProcessWindow::getFullPathFiles(QString pathType, QStringList& files)
{
    QString path = QApplication::applicationDirPath() + "/localfiles/";
    QString a0100 = archInfoHelper->getA0100();
    QJsonObject rqjson;
    ControlUtils::getCatalogAllFilesData(rqjson, a0100);
    QList<QVariant> list = rqjson.value("data").toVariant().toList();
    for (int i = 0; i < list.count(); i++)
    {
        QVariantMap map = list[i].toMap();
        // 检查图片明细表中E01ZB01字段如果比总页数大则不添加到下载队列中
        int nTotalPageNum = map["E01Z114"].toInt();
        int nCurPageNum = map["E01ZB01"].toInt();
        if (nCurPageNum > nTotalPageNum)
            continue;
        if (pathType == "sm")
        {
            if (map["d01zb02"].toString().trimmed() != "")
                files << FilesControl::GetLocalFilesDir() + "/" + map["d01zb02"].toString().trimmed() + "/" + map["e01zb03"].toString().trimmed();
        }
        else if (pathType == "ys")
        {
            if (map["d01zb03"].toString().trimmed() != "")
                files << FilesControl::GetLocalFilesDir() + "/" + map["d01zb03"].toString().trimmed().replace("sm", "ys") + "/" + map["e01zb03"].toString().trimmed();
        }
        else if (pathType == "gq")
        {
            if (map["d01zb04"].toString().trimmed() != "")
                files << FilesControl::GetLocalFilesDir() + "/" + map["d01zb04"].toString().trimmed() + "/" + map["e01zb03"].toString().trimmed();
        }
    }
}

void HDImageProcessWindow::sl_DownLoadAndUpLoadProgressOnClose()
{
    delete winDownUpProgress;
    winDownUpProgress = nullptr;
    on_u_treeView_catalog_clicked(ui->u_treeView_catalog->currentIndex());
}

//    上传原始图像
void HDImageProcessWindow::UpLoadYSFile()
{
    //遍历目录
    QString a0100 = archInfoHelper->getA0100();
    FileUpLoader* fileUploader = new FileUpLoader(this);
    UploadResult ret = fileUploader->uploadAll(nullptr, this, ui->u_treeView_catalog, a0100, 2);
    if (ret == UploadNoFile)
    {
        QArchScanGlobal::information(this, "提示", "没有需要上传的内容。");
    }
}
//    上传优化图像
void HDImageProcessWindow::UpLoadGQFile()
{
    //遍历目录
    QString a0100 = archInfoHelper->getA0100();
    FileUpLoader* fileUploader = new FileUpLoader(this);
    UploadResult ret = fileUploader->uploadAll(nullptr, this, ui->u_treeView_catalog, a0100, 3);
    if (ret == UploadNoFile)
    {
        QArchScanGlobal::information(this, "提示", "没有需要上传的内容。");
    }
}

void HDImageProcessWindow::ShowDownLoadAndUpLoadProgress(int type, QList<DownLoadAndUpLoadThreadTaskInfo*> tasklist)
{
    //上传下载窗口
    winDownUpProgress = new DownLoadAndUpLoadProgress(this);
    connect(winDownUpProgress, &DownLoadAndUpLoadProgress::si_CloseEvent, this, &HDImageProcessWindow::sl_DownLoadAndUpLoadProgressOnClose);
    connect(winDownUpProgress, &DownLoadAndUpLoadProgress::show_info, this, &HDImageProcessWindow::showMessage);
    winDownUpProgress->setAttribute(Qt::WA_DeleteOnClose);
    winDownUpProgress->showMaximized();
    winDownUpProgress->InitTaskListAndStartThread(type, tasklist);
}

void HDImageProcessWindow::on_u_treeView_catalog_clicked(const QModelIndex& index)
{
    QString a0100 = archInfoHelper->getA0100();
    Q_UNUSED(index)
    archInfoHelper->refreshSelectedCatalogInfo(selectedCatalogInfo);
    ui->u_label_statusbarTiShi->setText("");
    //获取文件列表
    //只有在1的时候才读，其它时候清空
    if (selectedCatalogInfo->LX == "1")
    {
        QJsonObject rqjson;
        ControlUtils::InitqsimFilesList_hdimage(rqjson, qsimFilesList, a0100
                                                , selectedCatalogInfo->ID
                                                , selectedCatalogInfo->E01Z114
                                                , selectedCatalogInfo->Code, selectedCatalogInfo->SXH);
        QStandardItem* item = qsimFilesList->item(0, 0);
        if (item != nullptr)
        {
            ui->u_tableView_filelist->setCurrentIndex(item->index());
            on_u_tableView_filelist_clicked(ui->u_tableView_filelist->currentIndex());
        }
        else
        {
            u_frame_orig->InitJPG("");
            u_frame_hd->InitJPG("");
        }
        QString lTiShi = "提示:  目录页数【%1】";
        lTiShi = lTiShi.arg(selectedCatalogInfo->E01Z114);
        ui->u_label_statusbarTiShi->setText(lTiShi);
    }
    else
    {
        ControlUtils::clearqsimFilesList_hdimage(qsimFilesList);
        u_frame_orig->InitJPG("");
        u_frame_hd->InitJPG("");
    }
    ui->u_tableView_filelist->setColumnWidth(1, 150);
}

void HDImageProcessWindow::on_pushButton_clicked()
{
    //下载文件
    FileDownloader* fileDownLoader = new FileDownloader(this);
    fileDownLoader->downloadAll(this, ui->u_treeView_catalog, archInfoHelper->getA0100(), 3);
}

void HDImageProcessWindow::on_pushButton_3_clicked()
{
    //上传原始图像
    UpLoadYSFile();
}

void HDImageProcessWindow::on_pushButton_4_clicked()
{
    //上传优化图像
    UpLoadGQFile();
}

void HDImageProcessWindow::on_u_tableView_filelist_clicked(const QModelIndex& index)
{
    QStandardItem* item = qsimFilesList->item(index.row(), 0);
    if (item != nullptr)
    {
        QString ys_allfilepath = item->data(Qt::UserRole + 2).toString();
        if (QFile::exists(ys_allfilepath))
        {
            u_frame_orig->InitJPG(ys_allfilepath);
            u_frame_orig->PicWin->imageView->showFitInHeight();
        }
        else
        {
            u_frame_orig->InitJPG("");
        }
        QString gq_allfilepath = item->data(Qt::UserRole + 3).toString();
        if (QFile::exists(gq_allfilepath))
        {
            u_frame_hd->InitJPG(gq_allfilepath);
            u_frame_hd->PicWin->imageView->showFitInHeight();
        }
        else
        {
            u_frame_hd->InitJPG("");
        }

    }
    else
    {
        u_frame_orig->InitJPG("");
        u_frame_hd->InitJPG("");
    }
}

//批量转优化
void HDImageProcessWindow::on_btnBatchClearImage_clicked()
{
    QString a0100 = archInfoHelper->getA0100();
    if (batChangeHdWindow != nullptr)
    {
        delete batChangeHdWindow;
        batChangeHdWindow = nullptr;
    }
    BatchangeHdWindow* batChangeHdWindow = new BatchangeHdWindow(this);
    QStringList srcFiles, destFiles;
    QJsonObject rjsonobj;
    ControlUtils::getGqRelativeFiles(rjsonobj, a0100);
    QList<QVariant> list = rjsonobj.value("data").toVariant().toList();
    for (int i = 0; i < list.count(); ++i)
    {
        QVariantMap msp = list[i].toMap();
        QString  ysAllPath = msp["d01zb03"].toString().replace("sm", "ys") + "/" + msp["e01zb03"].toString();
        srcFiles << ysAllPath;
        QString gqAllPath = ysAllPath.replace("ys", "gq");
        gqAllPath = gqAllPath.replace("sm", "gq");
        destFiles << gqAllPath;
    }
    batChangeHdWindow->srcFiles = srcFiles;
    batChangeHdWindow->destFiles = destFiles;
    batChangeHdWindow->show();
}

void HDImageProcessWindow::on_btnNextPage_clicked()
{
    int filelist_current_row = ui->u_tableView_filelist->currentIndex().row();
    QModelIndex filelist_row = ui->u_tableView_filelist->currentIndex().siblingAtRow(filelist_current_row + 1);
    if (filelist_row.isValid())
    {
        ui->u_tableView_filelist->setCurrentIndex(filelist_row);
        on_u_tableView_filelist_clicked(ui->u_tableView_filelist->currentIndex());
    }
    else
    {
        QStandardItem* qStandardItem = getNext_LX1_TreeViewNode(ui->u_treeView_catalog->currentIndex());
        if (qStandardItem != nullptr)
        {
            ui->u_treeView_catalog->setCurrentIndex(qStandardItem->index());
            on_u_treeView_catalog_clicked(ui->u_treeView_catalog->currentIndex());
        }
    }
}

void HDImageProcessWindow::on_btnPrePage_clicked()
{
    int filelist_current_row = ui->u_tableView_filelist->currentIndex().row();
    QModelIndex filelist_row = ui->u_tableView_filelist->currentIndex().siblingAtRow(filelist_current_row - 1);
    if (filelist_row.isValid())
    {
        ui->u_tableView_filelist->setCurrentIndex(filelist_row);
        on_u_tableView_filelist_clicked(ui->u_tableView_filelist->currentIndex());
    }
    else
    {
        QStandardItem* qStandardItem = getPre_LX1_TreeViewNode(ui->u_treeView_catalog->currentIndex());
        if (qStandardItem != nullptr)
        {
            ui->u_treeView_catalog->setCurrentIndex(qStandardItem->index());
            on_u_treeView_catalog_clicked(ui->u_treeView_catalog->currentIndex());
            if (ui->u_tableView_filelist->model()->rowCount() > 0)
            {
                QModelIndex filelist_last_row = ui->u_tableView_filelist->currentIndex().siblingAtRow(ui->u_tableView_filelist->model()->rowCount() - 1);
                if (filelist_last_row.isValid())
                {
                    ui->u_tableView_filelist->setCurrentIndex(filelist_last_row);
                    on_u_tableView_filelist_clicked(ui->u_tableView_filelist->currentIndex());
                }
            }
        }
    }
}

QStandardItem* HDImageProcessWindow::getNext_LX1_TreeViewNode(QModelIndex qBeginModelIndex)
{
    QModelIndex catalog_row = ui->u_treeView_catalog->indexBelow(qBeginModelIndex);
    while (catalog_row.isValid())
    {
        QStandardItem* item = qsimCatalog->itemFromIndex(catalog_row);
        if (item != nullptr)
        {
            QString lx = item->data(Qt::UserRole + 1).toString();
            if (lx == "1")
            {
                return item;
            }
            catalog_row = ui->u_treeView_catalog->indexBelow(item->index());
        }
        else
        {
            return nullptr;
        }
    }
    return nullptr;
}

QStandardItem* HDImageProcessWindow::getPre_LX1_TreeViewNode(QModelIndex qBeginModelIndex)
{
    QModelIndex catalog_row = ui->u_treeView_catalog->indexAbove(qBeginModelIndex);
    while (catalog_row.isValid())
    {
        QStandardItem* item = qsimCatalog->itemFromIndex(catalog_row);
        if (item != nullptr)
        {
            QString lx = item->data(Qt::UserRole + 1).toString();
            if (lx == "1")
            {
                return item;
            }
            catalog_row = ui->u_treeView_catalog->indexAbove(item->index());
        }
        else
        {
            return nullptr;
        }
    }
    return nullptr;
}

void HDImageProcessWindow::onControlTriggered()
{
    QString a0100 = archInfoHelper->getA0100();
    QAction* action = static_cast<QAction*>(sender());
    if (action->text() == "下载本类")
    {
        QModelIndex index = ui->u_treeView_catalog->currentIndex();
        QStandardItemModel* model = static_cast<QStandardItemModel*>(ui->u_treeView_catalog->model());
        QStandardItem* item = model->itemFromIndex(index);
        QList<DownLoadAndUpLoadThreadTaskInfo*> tasklist;
        ControlUtils::DownLoadDLFiles(item, tasklist, 2, a0100);
        ControlUtils::DownLoadDLFiles(item, tasklist, 3, a0100);
        if (tasklist.count() > 0)
        {
            ShowDownLoadAndUpLoadProgress(DownLoadAndUpLoadProgress::emDownLoad, tasklist);
        }
        qDeleteAll(tasklist);
    }
    else if (action->text() == "下载本件")
    {
        QModelIndex index = ui->u_treeView_catalog->currentIndex();
        QStandardItemModel* model = static_cast<QStandardItemModel*>(ui->u_treeView_catalog->model());
        QStandardItem* item = model->itemFromIndex(index);
        QList<DownLoadAndUpLoadThreadTaskInfo*> tasklist;
        ControlUtils::DownLoadXLFiles(item, tasklist, 2, a0100);
        ControlUtils::DownLoadXLFiles(item, tasklist, 3, a0100);
        if (tasklist.count() > 0)
        {
            ShowDownLoadAndUpLoadProgress(DownLoadAndUpLoadProgress::emDownLoad, tasklist);
        }
        qDeleteAll(tasklist);
    }
    else if (action->text() == "上传本类")
    {
        QModelIndex index = ui->u_treeView_catalog->currentIndex();
        QStandardItemModel* model = static_cast<QStandardItemModel*>(ui->u_treeView_catalog->model());
        QStandardItem* item = model->itemFromIndex(index);
        QList<DownLoadAndUpLoadThreadTaskInfo*> tasklist;
        QJsonObject rqjo1, rqjo2;
        bool f1 = ControlUtils::getUploadTaskListByLb(rqjo1, 0, a0100, item->data(Qt::UserRole + 7).toString(), tasklist);
        bool f2 = ControlUtils::getUploadTaskListByLb(rqjo2, 1, a0100, item->data(Qt::UserRole + 7).toString(), tasklist);
        if ((f1 == true) || (f2 == true))
            ShowDownLoadAndUpLoadProgress(DownLoadAndUpLoadProgress::emUpLoad, tasklist);
        else
            QArchScanGlobal::information(this, "提示", "没有需要上传的内容。");
    }
    else if (action->text() == "上传本件")
    {
        //遍历目录
        QString a0100 = archInfoHelper->getA0100();
        QModelIndex index = ui->u_treeView_catalog->currentIndex();
        QStandardItemModel* model = static_cast<QStandardItemModel*>(ui->u_treeView_catalog->model());
        QStandardItem* item = model->itemFromIndex(index);
        QString lx = item->data(Qt::UserRole + 1).toString();
        if (lx == "0")
        {
            return;
        }
        QList<DownLoadAndUpLoadThreadTaskInfo*> tasklist;
        QJsonObject rqjo1, rqjo2;
        bool f1 = ControlUtils::getUploadTaskListByLb(rqjo1, 0, a0100, item->data(Qt::UserRole + 2).toString(), tasklist);
        bool f2 = ControlUtils::getUploadTaskListByLb(rqjo2, 1, a0100, item->data(Qt::UserRole + 2).toString(), tasklist);
        if ((f1 == true) || (f2 == true))
            ShowDownLoadAndUpLoadProgress(DownLoadAndUpLoadProgress::emUpLoad, tasklist);
        else
            QArchScanGlobal::information(this, "提示", "没有需要上传的内容。");
        qDeleteAll(tasklist);
    }
    else if (action->text() == "文件目录修改")
    {
        QModelIndex index = ui->u_treeView_catalog->currentIndex();
        QStandardItemModel* model = static_cast<QStandardItemModel*>(ui->u_treeView_catalog->model());
        QStandardItem* item = model->itemFromIndex(index);
        QString lx = item->data(Qt::UserRole + 1).toString();
        if (lx == "0")
        {
            return;
        }
        QString id = item->data(Qt::UserRole + 2).toString();
        if (ControlUtils::EditArchInfo(item, id, this))
        {
            emit refreshArchInfos();
        }
    }
    else if (action->text() == "上传本页")
    {
        QModelIndex index = ui->u_tableView_filelist->currentIndex();
        if (index.row() < 0)
        {
            return;
        }
        QStandardItem* item = qsimFilesList->item(index.row());
        if (item != nullptr)
        {
            QList<DownLoadAndUpLoadThreadTaskInfo*> tasklist;
            ControlUtils::UpLoadOneFile(item, selectedCatalogInfo, tasklist, 2, a0100);
            ControlUtils::UpLoadOneFile(item, selectedCatalogInfo, tasklist, 3, a0100);
            if (tasklist.count() > 0)
            {
                ShowDownLoadAndUpLoadProgress(DownLoadAndUpLoadProgress::emUpLoad, tasklist);
            }
            qDeleteAll(tasklist);
        }
    }
    else if (action->text() == "下载本页")
    {
        QModelIndex filelistindex = ui->u_tableView_filelist->currentIndex();
        if (filelistindex.row() < 0)
        {
            return;
        }
        QStandardItem* filelistitem = qsimFilesList->item(filelistindex.row());
        QList<DownLoadAndUpLoadThreadTaskInfo*> tasklist;
        ControlUtils::DownLoadOneFile(filelistitem, tasklist, 2, a0100, selectedCatalogInfo->ID);
        ControlUtils::DownLoadOneFile(filelistitem, tasklist, 3, a0100, selectedCatalogInfo->ID);
        if (tasklist.count() > 0)
        {
            ShowDownLoadAndUpLoadProgress(DownLoadAndUpLoadProgress::emDownLoad, tasklist);
        }
        qDeleteAll(tasklist);
    }
    else if (action->text() == "清空本地全部文件")
    {
        if (QMessageBox::Yes == QMessageBox::question(this, "提示", "是否清空当前人员本地全部文件？清空后不可恢复。", QMessageBox::Yes | QMessageBox::No, QMessageBox::No))
        {
            FileSearcher fs;
            QString pathYS1 = FilesControl::GetLocalFilesPath_YS(a0100, "", "");
            fs.deleteFiles(pathYS1);
            QString pathGQ = FilesControl::GetLocalFilesPath_GQ(a0100, "", "");
            fs.deleteFiles(pathGQ);
            u_frame_orig->PicWin->imageView->clear();
            QStandardItemModel* model = static_cast<QStandardItemModel*>(ui->u_tableView_filelist->model());
            model->clear();
        }
    }
    else if (action->text() == "打开原始图片")
    {
        QModelIndex index = ui->u_tableView_filelist->currentIndex();
        QStandardItem* item = qsimFilesList->item(index.row());
        if (item == nullptr)
        {
            return;
        }
        QString path = item->data(Qt::UserRole + 2).toString();
        if (QFileInfo::exists(path))
        {
            QDesktopServices::openUrl(QUrl("file:///" + path));
        }
        else
        {
            QMessageBox::information(this, "提示", "图片不存在");
        }
    }
    else if (action->text() == "打开原始图片文件位置")
    {
        QModelIndex index = ui->u_tableView_filelist->currentIndex();
        QStandardItem* item = qsimFilesList->item(index.row());
        if (item == nullptr)
        {
            return;
        }
        QString path = item->data(Qt::UserRole + 2).toString();
        QFileInfo fileInfo(path);
        QDesktopServices::openUrl(QUrl("file:///" + fileInfo.absolutePath()));
    }
    else if (action->text() == "打开优化图片")
    {
        QModelIndex index = ui->u_tableView_filelist->currentIndex();
        QStandardItem* item = qsimFilesList->item(index.row());
        if (item == nullptr)
        {
            return;
        }
        QString path = item->data(Qt::UserRole + 3).toString();
        if (QFileInfo::exists(path))
        {
            QDesktopServices::openUrl(QUrl("file:///" + path));
        }
        else
        {
            QMessageBox::information(this, "提示", "图片不存在");
        }
    }
    else if (action->text() == "打开优化图片文件位置")
    {
        QModelIndex index = ui->u_tableView_filelist->currentIndex();
        QStandardItem* item = qsimFilesList->item(index.row());
        if (item == nullptr)
        {
            return;
        }
        QString path = item->data(Qt::UserRole + 3).toString();
        QFileInfo fileInfo(path);
        QDesktopServices::openUrl(QUrl("file:///" + fileInfo.absolutePath()));
    }
    else if (action->text() == "本类转优化")
    {
        QModelIndex index = ui->u_treeView_catalog->currentIndex();
        QStandardItemModel* model = static_cast<QStandardItemModel*>(ui->u_treeView_catalog->model());
        QStandardItem* item = model->itemFromIndex(index);
        QString lx = item->data(Qt::UserRole + 1).toString();
        if (lx != "0")
        {
            return;
        }
        QStringList srcFiles;
        QStringList destFiles;
        ControlUtils::YsToGqDLFiles(item, a0100, srcFiles, destFiles);
        if (srcFiles.count() > 0)
        {
            if (batChangeHdWindow != nullptr)
            {
                delete batChangeHdWindow;
                batChangeHdWindow = nullptr;
            }
            BatchangeHdWindow* batChangeHdWindow = new BatchangeHdWindow(this);
            batChangeHdWindow->srcFiles = srcFiles;
            batChangeHdWindow->destFiles = destFiles;
            batChangeHdWindow->show();
        }
    }
    else if (action->text() == "本件转优化")
    {
        QModelIndex index = ui->u_treeView_catalog->currentIndex();
        QStandardItemModel* model = static_cast<QStandardItemModel*>(ui->u_treeView_catalog->model());
        QStandardItem* item = model->itemFromIndex(index);
        QString lx = item->data(Qt::UserRole + 1).toString();
        if (lx == "0")
        {
            return;
        }
        QString id = item->data(Qt::UserRole + 2).toString();
        QStringList srcFiles;
        QStringList destFiles;
        ControlUtils::YsToGqXLFiles(a0100, id, srcFiles, destFiles);
        if (srcFiles.count() > 0)
        {
            if (batChangeHdWindow != nullptr)
            {
                delete batChangeHdWindow;
                batChangeHdWindow = nullptr;
            }
            BatchangeHdWindow* batChangeHdWindow = new BatchangeHdWindow(this);
            batChangeHdWindow->srcFiles = srcFiles;
            batChangeHdWindow->destFiles = destFiles;
            batChangeHdWindow->show();
        }
    }
    else if (action->text() == "漏页转优化")
    {
        QString a0100 = archInfoHelper->getA0100();
        if (batChangeHdWindow != nullptr)
        {
            delete batChangeHdWindow;
            batChangeHdWindow = nullptr;
        }
        BatchangeHdWindow* batChangeHdWindow = new BatchangeHdWindow(this);
        QStringList srcFiles, destFiles;
        QJsonObject rjsonobj;
        ControlUtils::getGqRelativeFiles(rjsonobj, a0100);
        QList<QVariant> list = rjsonobj.value("data").toVariant().toList();
        for (int i = 0; i < list.count(); ++i)
        {
            QVariantMap msp = list[i].toMap();
            srcFiles <<  msp["d01zb03"].toString().replace("sm", "ys") + "/" + msp["e01zb03"].toString();
            destFiles <<  msp["d01zb03"].toString().replace("ys", "gq") + "/" + msp["e01zb03"].toString();
        }
        batChangeHdWindow->srcFiles = srcFiles;
        batChangeHdWindow->destFiles = destFiles;
        batChangeHdWindow->skipExists = true;
        batChangeHdWindow->show();
    }
    else if (action->text() == "打印")
    {
        PrintImagesDialog* printImagesDialog = new PrintImagesDialog(this);
        printImagesDialog->setPageRange(1, qsimFilesList->rowCount());
        printImagesDialog->setSourceFrmType(PrintImagesDialog::SourceViewHD);
        if (printImagesDialog->exec() == QDialog::Accepted)
        {
            QStringList files;
            switch (printImagesDialog->m_printType)
            {
            case 0:
            {
                QModelIndex index = ui->u_tableView_filelist->currentIndex();
                QStandardItem* item = qsimFilesList->item(index.row());
                if (item == nullptr)
                {
                    break;
                }
                QString path;
                if (printImagesDialog->m_imageType == 0)
                {
                    path = item->data(Qt::UserRole + 2).toString();
                }
                else
                {
                    path = item->data(Qt::UserRole + 3).toString();
                }
                QFileInfo info(path);
                if (info.exists())
                {
                    files << path;
                }
                break;
            }
            case 1:
            {
                int minPage = printImagesDialog->m_pagenum_min;
                int maxPage = printImagesDialog->m_pagenum_max;
                for (int i = minPage - 1; i < maxPage; i++)
                {
                    QString path;
                    QStandardItem* item = qsimFilesList->item(i);
                    if (printImagesDialog->m_imageType == 0)
                    {
                        path = item->data(Qt::UserRole + 2).toString();
                    }
                    else
                    {
                        path = item->data(Qt::UserRole + 3).toString();
                    }
                    QFileInfo info(path);
                    if (info.exists())
                    {
                        files << path;
                    }
                }
                break;
            }
            case 2:
            {
                if (printImagesDialog->m_imageType == 0)
                {
                    getFullPathFiles("ys", files);
                }
                else
                {
                    getFullPathFiles("gq", files);
                }
                break;
            }
            }
            if (files.count() < 1)
            {
                QArchScanGlobal::information(this, "提示", "本地没有档案图片!");
            }
            printImagesDialog->printViewImages(files, printImagesDialog->m_blackAndWhite, printImagesDialog->m_duplexMode);
        }
        delete printImagesDialog;
    }
}

void HDImageProcessWindow::on_btnClear_clicked()
{
    QString a0100 = archInfoHelper->getA0100();
    if (0 == QMessageBox::question(this, "提示", "您确定清空本卷图像吗?", "是", "否", "", 0, 1))
    {
        QString datafilepath_ys1 = FilesControl::GetLocalFilesPath_YS(a0100, "", "");
        QArchScanGlobal::clearFolder(datafilepath_ys1);
        QString datafilepath_gq = FilesControl::GetLocalFilesPath_GQ(a0100, "", "");
        QDir qdir1(datafilepath_gq);
        if (qdir1.exists(datafilepath_gq))
        {
            if (!qdir1.removeRecursively())
            {
                return;
            }
        }
        on_u_treeView_catalog_clicked(ui->u_treeView_catalog->rootIndex());
    }
}

void HDImageProcessWindow::on_u_treeView_catalog_customContextMenuRequested(const QPoint& pos)
{
    Q_UNUSED(pos)
    QModelIndex index = ui->u_treeView_catalog->currentIndex();
    QStandardItemModel* model = static_cast<QStandardItemModel*>(ui->u_treeView_catalog->model());
    QStandardItem* item = model->itemFromIndex(index);
    QString lx = item->data(Qt::UserRole + 1).toString();
    QMenu* cmenu = new QMenu(ui->u_treeView_catalog);
    if (lx == "0")
    {
        QString actionNames[3] = { "下载本类", "上传本类", "本类转优化" };
        for (int i = 0; i < 3; i++)
        {
            QAction* act = cmenu->addAction(actionNames[i]);
            connect(act, SIGNAL(triggered()), this, SLOT(onControlTriggered()));
        }
    }
    else if (lx == "1")
    {
        QString actionNames[4] = { "下载本件", "上传本件", "本件转优化", "文件目录修改" };
        for (int i = 0; i < 4; i++)
        {
            QAction* act = cmenu->addAction(actionNames[i]);
            connect(act, SIGNAL(triggered()), this, SLOT(onControlTriggered()));
        }
    }
    QAction* action20 = cmenu->addAction("漏页转优化");
    QAction* action21 = cmenu->addAction("打印");
    connect(action20, SIGNAL(triggered()), this, SLOT(onControlTriggered()));
    connect(action21, SIGNAL(triggered()), this, SLOT(onControlTriggered()));
    cmenu->exec(QCursor::pos());
}

void HDImageProcessWindow::on_u_tableView_filelist_customContextMenuRequested(const QPoint& pos)
{
    Q_UNUSED(pos)
    QModelIndex filelistindex = ui->u_tableView_filelist->currentIndex();
    QMenu* cmenu = new QMenu(ui->u_treeView_catalog);
    if (filelistindex.row() < 0)
    {
        QAction* action1 = cmenu->addAction("下载本件");
        connect(action1, SIGNAL(triggered()), this, SLOT(onControlTriggered()));
    }
    else
    {
        QStandardItem* filelistitem = qsimFilesList->item(filelistindex.row());
        if (filelistitem != nullptr)
        {
            QString actionNames[8] = { "下载本页", "上传本页", "下载本件", "上传本件",
                                       "打开原始图片", "打开原始图片文件位置",
                                       "打开优化图片", "打开优化图片文件位置"
                                     };
            for (int i = 0; i < 8; i++)
            {
                QAction* act = cmenu->addAction(actionNames[i]);
                connect(act, SIGNAL(triggered()), this, SLOT(onControlTriggered()));
            }
        }
    }
    cmenu->exec(QCursor::pos());
}

bool HDImageProcessWindow::eventFilter(QObject* obj, QEvent* e)
{
    Q_UNUSED(obj)
    if (e->type() == QEvent::KeyPress)
    {
        this->keyPressEvent(static_cast<QKeyEvent*>(e));
        if (static_cast<QKeyEvent*>(e)->key() == Qt::Key_Delete || static_cast<QKeyEvent*>(e)->key() == Qt::Key_Space)
        {
            return true;
        }
    }
    return false;
}

void HDImageProcessWindow::keyPressEvent(QKeyEvent* event)
{
    switch (event->key())
    {
    case Qt::Key_Delete:
    {
        //        delete_u_tableView_filelist_currentitem();
        break;
    }
    case Qt::Key_Space:
    {
        if (event->modifiers() == Qt::ShiftModifier)
            on_btnPrePage_clicked();
        else
            on_btnNextPage_clicked();
        break;
    }
    }
}

void HDImageProcessWindow::onResize()
{
    resizeEvent(nullptr);
}
void HDImageProcessWindow::onSelectPerson(QTableView* tableView, QString id, QString xm, QString cardNum)
{
    Q_UNUSED(tableView)
    disconnect(searchPersonInfoExForm->searchPersonInfoForm, SIGNAL(onSelectPerson(QTableView*, QString, QString, QString)), this,
               SLOT(onSelectPerson(QTableView*, QString, QString, QString)));
    archInfoHelper->loadData(id);
    u_frame_orig->m_A0100 = id;
    QString lwindowTitle = "图像处理 姓名: %1  身份证号:%2";
    lwindowTitle = lwindowTitle.arg(xm).arg(cardNum);
    setWindowTitle(lwindowTitle);
    connect(searchPersonInfoExForm->searchPersonInfoForm, SIGNAL(onSelectPerson(QTableView*, QString, QString, QString)), this,
            SLOT(onSelectPerson(QTableView*, QString, QString, QString)));
}

void HDImageProcessWindow::onLoadData(QString a0100)
{
    u_frame_orig->m_A0100 = a0100;
}

void HDImageProcessWindow::onSrcImageViewChangeOperateType(ImageView::ImageOperateType operateType)
{
    ui->label_2a->setText("原始图像数据      当前操作：[" + u_frame_orig->PicWin->imageView->getOperatorTypeName(operateType) + "]");
}

void HDImageProcessWindow::onHdImageViewChangeOperateType(ImageView::ImageOperateType operateType)
{
    ui->label_2s->setText("优化图像数据      当前操作：[" + u_frame_hd->PicWin->imageView->getOperatorTypeName(operateType) + "]");
}
void HDImageProcessWindow::showMessage(int type, QString title, QString content)
{
    if (type == 0)
    {
        QMessageBox::information(nullptr, title, content);
    }
}
