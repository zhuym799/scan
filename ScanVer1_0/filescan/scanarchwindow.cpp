#include "scanarchwindow.h"
#include "ui_scanarchwindow.h"
#include <unistd.h>
#include <QAction>
#include <QDesktopServices>
#include <QEvent>
#include <QKeyEvent>
#include <QMenu>
#include <QMessageBox>
#include <QStandardItemModel>
#include <QTreeView>
#include <QVBoxLayout>
#include <QJsonObject>
#include <QJsonArray>
#include <QFileDialog>
#include "src/umsp-hfes-client/cryptcore.h"
#include "src/global/controlutils.h"
#include "src/global/filedownloader.h"
#include "src/global/fileuploader.h"
#include "src/global/qarchscanglobal.h"
#include "src/global/skincontrolor.h"
#include "src/ui/business/personinfo/archinfohelper.h"
#include "src/ui/business/common/searchpersoninfoexform.h"
#include "src/ui/business/filescan/scanpictureprocesswindow.h"
#include "src/ui/business/common/downanduploadprogress.h"
#include "src/ui/business/filescan/scannercontinueform.h"
#include "src/common/filescontrol.h"
#include "src/scannersdk/sanecore.h"
#ifdef QT_WIN
#include "src/scannersdk/scannerconfig.h"
#include "src/scannersdk/dtwaincore.h"
#else
#include "src/scannersdk/scanner_operation.h"
#include "src/scannersdk/scannerconfig_sane.h"
#endif

#ifdef QT_WIN
DTwainAPP* ScannerAPP::m_ScannerCore = nullptr;
#else
Scanner_Operation* ScannerAPP::m_ScannerCore = nullptr;
#endif


ScanArchWindow::ScanArchWindow(QWidget* parent)
    : QMainWindow(parent)
    , ui(new Ui::ScanArchWindow)
{
    ui->setupUi(this);
    this->setStyleSheet(SkinControlor::skinText);
    check_ini();
    setting_init();
    selectedCatalogInfo = new TSelectedCatalogInfo();
    QArchScanGlobal::initSelectedCatalogInfo(selectedCatalogInfo);
    qsimCatalog = new QStandardItemModel(ui->u_treeView_catalog);
    archInfoHelper = new ArchInfoHelper(this);
    archInfoHelper->init(ui->u_treeView_catalog, qsimCatalog);
    //目录
    connect(ui->u_treeView_catalog, &HxQTreeView::si_dropEvent, this, &ScanArchWindow::sl_treeView_catalog_dropEvent);
//    connect(ui->u_treeView_catalog->selectionModel(), &QItemSelectionModel::currentChanged, this, &ScanArchWindow::on_u_treeView_catalog_currentChanged);
    ui->u_treeView_catalog->installEventFilter(this);
    //文件列表
    qsimFilesList = new QStandardItemModel(ui->u_tableView_filelist);
    ui->u_tableView_filelist->setModel(qsimFilesList);
    QStringList labels;
    labels << "名称"
           << "是否上传";
    qsimFilesList->setHorizontalHeaderLabels(labels);
    ui->u_tableView_filelist->setColumnWidth(0, 100);
    ui->u_tableView_filelist->setColumnWidth(1, 200);
    connect(ui->u_tableView_filelist->selectionModel(), &QItemSelectionModel::currentChanged, this, &ScanArchWindow::on_u_tableView_filelist_currentChanged);
    ui->u_tableView_filelist->installEventFilter(this);
    qsimFilesList->horizontalHeaderItem(0)->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    qsimFilesList->horizontalHeaderItem(1)->setTextAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    //校验列表
    qsimUploadCheckList = new QStandardItemModel(ui->u_tableView_uploadCheck);
    ui->u_tableView_uploadCheck->setModel(qsimUploadCheckList);
    qsimUploadCheckList->setColumnCount(2);
    qsimUploadCheckList->setHeaderData(0, Qt::Horizontal, "材料名称");
    qsimUploadCheckList->setHeaderData(1, Qt::Horizontal, "结果");
    //设置选中时为整行选中
    ui->u_treeView_catalog->setSelectionBehavior(QAbstractItemView::SelectRows);
    //扫描仪设置
    QVBoxLayout* u_QVBoxLayout_ScannerConfig = new QVBoxLayout();
    u_QVBoxLayout_ScannerConfig->setContentsMargins(1, 1, 1, 1);
    ui->wgtScanClient->setLayout(u_QVBoxLayout_ScannerConfig);
    ui->wgtScanClient->setStyleSheet(SkinControlor::skinText);
#ifdef QT_WIN
    m_ScannerConfig = new ScannerConfig(ui->wgtScanClient);
#else
    m_ScannerConfig = new ScannerConfig_Sane(ui->wgtScanClient);
#endif
    u_QVBoxLayout_ScannerConfig->addWidget(m_ScannerConfig);
    ui->u_widget_ScannerSetting->setVisible(false);
    m_ScannerConfig->savConfig();

    //原始图像区域 frame OrigPictureProcessWindow
    QVBoxLayout* u_QVBoxLayout_ori = new QVBoxLayout();
    u_QVBoxLayout_ori->setContentsMargins(1, 1, 1, 1);
    ui->wgtImgYuanshi->setLayout(u_QVBoxLayout_ori);
    u_frame_scan = new ScanPictureProcessWindow(ui->wgtImgYuanshi);
    connect(u_frame_scan, &ScanPictureProcessWindow::si_on_u_pushButton_scan_clicked, this, &ScanArchWindow::sl_on_u_pushButton_scan_clicked);
    u_frame_scan->setWindowState(Qt::WindowMaximized);
    u_QVBoxLayout_ori->addWidget(u_frame_scan);
    u_frame_scan->PicWin->imageView->installEventFilter(this);
    searchPersonInfoExForm = new SearchPersonInfoExForm(ui->u_widget_client);
    connect(searchPersonInfoExForm, SIGNAL(onFormResize()), this, SLOT(onResize()));
    connect(searchPersonInfoExForm->searchPersonInfoForm, SIGNAL(onSelectPerson(QTableView*, QString, QString, QString)), this,
            SLOT(onSelectPerson(QTableView*, QString, QString, QString)));
    ui->u_widget_uploadCheck->setHidden(true);
    m_lastclickedtime = QDateTime::currentDateTime(); //获取当前时间
    m_lastclickedT = m_lastclickedtime.toTime_t(); //将当前时间转为时间戳
    connect(u_frame_scan->PicWin->imageView, &ImageView::onChangeOperateType,
            this, &ScanArchWindow::onSrcImageViewChangeOperateType);
    ui->btnPrePage->setToolTip("Shift+空格键");
    ui->btnNextPage->setToolTip("空格键");
    ui->u_tableView_filelist->setSelectionMode(QAbstractItemView::ExtendedSelection);
    ui->u_tableView_filelist->setSelectionBehavior(QAbstractItemView::SelectRows);

#ifdef QT_WIN
    ScannerAPP::m_ScannerCore->setCallBackFunShowOnePic(ScanArchWindow::CallBackFunShowOnePicFromScannerFrm);
#endif
    GCurScanFrmWidget = this;
}

ScanArchWindow::~ScanArchWindow()
{
    delete ui;
}

void ScanArchWindow::check_ini()
{
    QString configAllFilePath = QGuiApplication::applicationDirPath() + "/configs/scansetting.ini";
    if (!QFile::exists(configAllFilePath))
    {
        qDebug() << "loadIniFile scansetting.ini not exists" << endl;
        QSettings*  lini = new QSettings(configAllFilePath, QSettings::IniFormat);
        lini->setIniCodec("UTF8");
        lini ->beginGroup("scansetting");//功能
        auto_split = lini->value("auto_split",  true).toBool();
        auto_scan = lini->value("auto_scan", false).toBool();
        interval_time = lini->value("interval_time", "5").toString();
        lini->endGroup();
    }
}
void ScanArchWindow::setting_init()
{
    QString configAllFilePath = QGuiApplication::applicationDirPath() + "/configs/scansetting.ini";
    if (QFile::exists(configAllFilePath))
    {
        qDebug() << "loadIniFile scansetting.ini exists" << endl;
        QSettings*  lini = new QSettings(configAllFilePath, QSettings::IniFormat);
        lini->setIniCodec("UTF8");
        lini ->beginGroup("scansetting");//功能
        auto_split = lini->value("auto_split",  true).toBool();
        auto_scan = lini->value("auto_scan", false).toBool();
        interval_time = lini->value("interval_time", "5").toString();
        lini->endGroup();
    }
    ui->u_checkBox_checkCanScanNode->setChecked(auto_split);
    ui->u_checkBox_AutoScan->setChecked(auto_scan);
    ui->u_doubleSpinBox_AutoScanM->setValue(interval_time.toDouble());
}

void ScanArchWindow::save_settting()
{
    QString configAllFilePath = QGuiApplication::applicationDirPath() + "/configs/scansetting.ini";
    //没有检测到先写一遍默认值
    if (QFile::exists(configAllFilePath))
    {
        QSettings*  lini = new QSettings(configAllFilePath, QSettings::IniFormat);
        lini->setIniCodec("UTF8");
        lini ->beginGroup("scansetting");//功能
        lini->setValue("auto_split", auto_split); //自动分件
        lini->setValue("auto_scan", auto_scan);
        lini->setValue("interval_time", interval_time); //扫描时间间隔
        lini->endGroup();
    }
}
void ScanArchWindow::setSpinBoxValues(QStandardItem* item)
{
    ui->u_spinBox_totalpage->setValue(item == nullptr ? 0 : item->data(Qt::UserRole + 2).toInt());
    ui->u_spinBox_scanedPage->setValue(item == nullptr ? 0 : item->data(Qt::UserRole + 3).toInt());
    ui->u_spinBox_unscanedPage->setValue(item == nullptr ? 0 : item->data(Qt::UserRole + 4).toInt());
    ui->u_spinBox_morescanedPage->setValue(item == nullptr ? 0 : item->data(Qt::UserRole + 5).toInt());
}

void ScanArchWindow::setControlEnable(bool b0, bool b1, bool b2, bool b3)
{
    ui->u_pushButton_editUploadCheck->setEnabled(b0);
    ui->u_pushButton_saveUploadCheck->setEnabled(b1);
    ui->u_pushButton_cancelUploadCheck->setEnabled(b2);
    ui->u_spinBox_totalpage->setEnabled(b3);
}

void ScanArchWindow::setLoadedFinish(bool bFinished)
{
    loadFinished = bFinished;
}

void ScanArchWindow::closeEvent(QCloseEvent* event)
{
    if (!loadFinished)
        event->ignore();
    else
        QMainWindow::closeEvent(event);
}

void ScanArchWindow::sl_DownLoadAndUpLoadProgressOnClose()
{
    delete winDownUpProgress;
    winDownUpProgress = nullptr;
    on_u_treeView_catalog_clicked(ui->u_treeView_catalog->currentIndex());
}

void ScanArchWindow::UpLoadFile()
{
    //遍历目录
    clearUploadCheckList();
    QString a0100 = archInfoHelper->getA0100();
    bool boolCoverServerPictureForUpload = false;
    FileUpLoader* fileUploader = new FileUpLoader(this);
    fileUploader->boolCoverServerPictureForUpload = boolCoverServerPictureForUpload;
    UploadResult ret = fileUploader->uploadAll(qsimUploadCheckList, this, ui->u_treeView_catalog, a0100, 1);
    if (ret == UploadError)
    {
        ui->u_widget_uploadCheck->setVisible(true);
        if (qsimUploadCheckList->rowCount() > 0)
        {
            ui->u_tableView_uploadCheck->setCurrentIndex(qsimUploadCheckList->item(0)->index());
            on_u_tableView_uploadCheck_clicked(ui->u_tableView_uploadCheck->currentIndex());
        }
    }
    if (ret != uploadSuccess)
    {
        delete fileUploader;
    }
}

void ScanArchWindow::ShowDownLoadAndUpLoadProgress(int type, QList<DownLoadAndUpLoadThreadTaskInfo*> tasklist, bool aboolCoverServerPictureForUpload)
{
    //上传下载窗口
    winDownUpProgress = new DownLoadAndUpLoadProgress(this);
    winDownUpProgress->boolFrmCoverServerPictureForUpload = aboolCoverServerPictureForUpload;
    connect(winDownUpProgress, &DownLoadAndUpLoadProgress::si_CloseEvent, this, &ScanArchWindow::sl_DownLoadAndUpLoadProgressOnClose);
    connect(winDownUpProgress, &DownLoadAndUpLoadProgress::show_info, this, &ScanArchWindow::showMessage);

    winDownUpProgress->setAttribute(Qt::WA_DeleteOnClose);
    winDownUpProgress->showMaximized();
    winDownUpProgress->InitTaskListAndStartThread(type, tasklist);
}

void ScanArchWindow::on_u_treeView_catalog_clicked(const QModelIndex& index)
{
    Q_UNUSED(index)
    QString a0100 = archInfoHelper->getA0100();
    QArchScanGlobal::log("on_u_treeView_catalog_clicked----1---");
    archInfoHelper->refreshSelectedCatalogInfo(selectedCatalogInfo);
    ui->u_label_statusbarTiShi->setText("");
    //只有在1的时候才读，其它时候清空
    if (selectedCatalogInfo->LX == "1")
    {
        u_frame_scan->CanScan(true);
        ControlUtils::InitqsimFilesList_scan(qsimFilesList, a0100, selectedCatalogInfo->ID, selectedCatalogInfo->Code, selectedCatalogInfo->SXH);
        ui->u_tableView_filelist->setColumnWidth(0, 100);
        ui->u_tableView_filelist->setColumnWidth(1, 200);
        QStandardItem* item = qsimFilesList->item(0, 0);
        if (item != nullptr)
        {
            ui->u_tableView_filelist->setCurrentIndex(item->index());
            on_u_tableView_filelist_clicked(ui->u_tableView_filelist->currentIndex());
        }
        else
        {
            u_frame_scan->InitJPG("");
        }
        //点击时检查单项 扫描页数和未扫页数
        uploadCheckSingleDirectory(selectedCatalogInfo->ID);
    }
    else
    {
        ControlUtils::clearqsimFilesList_scan(qsimFilesList);
        ui->u_tableView_filelist->setColumnWidth(0, 100);
        ui->u_tableView_filelist->setColumnWidth(1, 200);
        u_frame_scan->CanScan(false);
        u_frame_scan->InitJPG("");
    }
}

void ScanArchWindow::on_u_tableView_filelist_clicked(const QModelIndex& index)
{
    QStandardItem* item = qsimFilesList->item(index.row(), 0);
    if (item != nullptr)
    {
        QString sm_allfilepath = item->data(Qt::UserRole + 1).toString();
        QArchScanGlobal::log("on_u_tableView_filelist_clicked----1--" + sm_allfilepath);
        u_frame_scan->InitJPG(sm_allfilepath);
    }
}

/*选择并设置扫描仪*/
void ScanArchWindow::on_btnScanner_clicked()
{
    bool isVisible = ui->u_widget_ScannerSetting->isVisible();
    ui->u_widget_ScannerSetting->setVisible(!isVisible);
}

void ScanArchWindow::sl_on_u_pushButton_scan_clicked()
{
    //防止过快点击,如果两次点击小于与等1秒说明点击过快
    if (bTClickLessthan(2))
    {
        return;
    }
    if (!mutex_doScan.tryLock(0))
    {
        return;
    }
    if (selectedCatalogInfo->LX == "0")
    {
        mutex_doScan.unlock();
        return;
    }
#ifdef QT_WIN
    bool boolHightScan = m_ScannerConfig->sc_ScanType;
#else
    bool boolHightScan = m_ScannerConfig->sc_ScanType;
#endif
    if (m_ScannerConfig->sc_DefaultScan_Index < 0)
    {
        QMessageBox::warning(this, "警告", "请在扫描仪配置中选择可用扫描仪");
        return;
    }
    if (boolHightScan)
    {
        doScanToFileList(); //高扫
    }
    else
    {
        doScanToFile();
    }
}

//高速扫描
void ScanArchWindow::doScanToFileList()
{
    QString lAllFilePath = qApp->applicationDirPath() + "/scantmp/" + QUuid::createUuid().toString(QUuid::WithoutBraces) + ".jpg";
    QString szFilePathDir = qApp->applicationDirPath() + "/scantmp/";
#if QT_WIN
    QString lListFileName, FilesCount;
#else
    QStringList lListFileName;
    QString FilesCount;
#endif
    //int index;//, iPicCount;
#if QT_WIN
    GCurScanFrmWidget = this;
    ScannerAPP::m_ScannerCore->setCallBackForScanToDir();
    if (ScannerAPP::m_ScannerCore->doScanToDir(lAllFilePath, lListFileName, FilesCount))
#else
    int type = 0;
    if (ScannerAPP::m_ScannerCore->scannerlist[m_ScannerConfig->sc_DefaultScan_Index]->get_scanner_name().contains("Pantum DS-230 Scanner"))
    {
        type = 0;
    }
    else
    {
        type = 1;
    }
    if (ScannerAPP::m_ScannerCore->doScanToDir(type, ScannerAPP::m_ScannerCore->scannerlist[m_ScannerConfig->sc_DefaultScan_Index]->get_saneHandle(), szFilePathDir, lListFileName, FilesCount))
#endif
    {
#ifndef QT_WIN
        //回调函数中处理图片显示
        qDebug() << "FilesCount is " << FilesCount.toInt() << endl;
        for (int i = 0; i < FilesCount.toInt(); i++)
        {
            qDebug() << "szFilePathDir is " << szFilePathDir << endl;
            qDebug() << "lListFileName :" << lListFileName.at(i) << endl;
            lAllFilePath = lListFileName.at(i);
#ifdef QT_USE_UMSP_CryptCore
            CryptCore* m_cryptcore = CryptCore::GetCryptCore();
            if (!m_cryptcore->u_encryptFileSelf(lAllFilePath))
            {
                mutex_doScan.unlock();
                return;
            }
#endif
            AddTwainJpg(lAllFilePath);
            //执行到这里图片扫描完成需要及时解锁
            mutex_doScan.unlock();
            //连续扫描窗口
            if (ui->u_checkBox_AutoScan->isChecked())
            {
                if (ScannerContinueForm::DoShow(this, static_cast<int>(ui->u_doubleSpinBox_AutoScanM->value())))
                {
                    sl_on_u_pushButton_scan_clicked();
                }
            }
        }
#endif
    }
    else
    {
        QArchScanGlobal::information(this, this->windowTitle(), "扫描失败，请重试");
    }
    mutex_doScan.unlock();
}

void ScanArchWindow::UpdateShowUIForHighScan(QString AllFilePath)
{
    if (selectedCatalogInfo->LX == "0")
        on_btnNextCata_clicked();
    QString timestamp = QArchScanGlobal::createGuid();
    QString destAllFilePath = QArchScanGlobal::getFileAbsolutePath(AllFilePath) + "/" + timestamp + ".jpg";
    //开始添加图片
    if (QFileInfo::exists(AllFilePath))
    {
#ifdef QT_USE_UMSP_CryptCore
        CryptCore* m_cryptcore = CryptCore::GetCryptCore();
        if (!m_cryptcore->u_encryptFile(AllFilePath, destAllFilePath))
        {
            return;
        }
#endif
        if (QFileInfo::exists(destAllFilePath))
            AddTwainJpg(destAllFilePath);
    }
}

void ScanArchWindow::CallBackFunShowOnePicFromScannerFrm(int FileIndex)
{
    QString szFilePathDir = qApp->applicationDirPath() + "/scantmp/";
    QString lAllFilePath = szFilePathDir + QArchScanGlobal::lengthCoverByIsLeft(QString::number(FileIndex), 3, true) + ".jpg";
    if (QFileInfo::exists(lAllFilePath))
    {
        ScanArchWindow* scanfrm = static_cast<ScanArchWindow*>(GCurScanFrmWidget);
        scanfrm->UpdateShowUIForHighScan(lAllFilePath);
    }
}


//平板扫描
void ScanArchWindow::doScanToFile()
{
    //平板扫描和高速扫描，并显示图片
    QString lAllFilePath = qApp->applicationDirPath() + "/scantmp/" + QUuid::createUuid().toString(QUuid::WithoutBraces) + ".jpg";
    //平板扫描
#if QT_WIN
    if (ScannerAPP::m_ScannerCore->doScanToFile(lAllFilePath))
#else
    if (ScannerAPP::m_ScannerCore->doScanToFile(ScannerAPP::m_ScannerCore->scannerlist[m_ScannerConfig->sc_DefaultScan_Index]->get_saneHandle(), lAllFilePath))
#endif
    {
#ifdef QT_USE_UMSP_CryptCore
        CryptCore* m_cryptcore = CryptCore::GetCryptCore();
        if (!m_cryptcore->u_encryptFileSelf(lAllFilePath))
        {
            mutex_doScan.unlock();
            return;
        }
#endif
        AddTwainJpg(lAllFilePath);
        //执行到这里图片扫描完成需要及时解锁
        mutex_doScan.unlock();
        //连续扫描窗口
        if (ui->u_checkBox_AutoScan->isChecked())
        {
            if (ScannerContinueForm::DoShow(this, static_cast<int>(ui->u_doubleSpinBox_AutoScanM->value())))
            {
                sl_on_u_pushButton_scan_clicked();
            }
        }
    }
    else
    {
        mutex_doScan.unlock();
        QArchScanGlobal::information(this, this->windowTitle(), "扫描失败，请重试");
    }
}

void ScanArchWindow::AddTwainJpg(QString AllFilePath)
{
    u_frame_scan->InitJPG(AllFilePath);
    u_tableView_filelist_add();
    QStandardItem* item = qsimFilesList->item(qsimFilesList->rowCount() - 1, 0);
    if (item == nullptr)
    {
        return;
    }
    ui->u_tableView_filelist->setCurrentIndex(item->index());
    QFile file(AllFilePath);
    if (file.exists())
    {
        file.remove();
    }
    if (ui->u_checkBox_checkCanScanNode->isChecked())
    {
        if (ui->u_tableView_filelist->model()->rowCount() == selectedCatalogInfo->E01Z114)
        {
            usleep(500);
            on_btnNextPage_clicked();
        }
    }
    uploadCheckSingleDirectory(selectedCatalogInfo->ID);
    this->show();
    this->activateWindow();
}

void ScanArchWindow::on_btnPrePage_clicked()
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
        QStandardItem* qStandardItem = archInfoHelper->getPre_LX1_TreeViewNode(ui->u_treeView_catalog->currentIndex());
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

void ScanArchWindow::on_btnNextPage_clicked()
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
        QStandardItem* qStandardItem = archInfoHelper->getNext_LX1_TreeViewNode(ui->u_treeView_catalog->currentIndex());
        if (qStandardItem != nullptr)
        {
            ui->u_treeView_catalog->setCurrentIndex(qStandardItem->index());
            on_u_treeView_catalog_clicked(ui->u_treeView_catalog->currentIndex());
        }
    }
}

void ScanArchWindow::on_btnNextCata_clicked()
{
    QStandardItem* qStandardItem = archInfoHelper->getNext_LX1_TreeViewNode(ui->u_treeView_catalog->currentIndex());
    if (qStandardItem != nullptr)
    {
        ui->u_treeView_catalog->setCurrentIndex(qStandardItem->index());
        on_u_treeView_catalog_clicked(ui->u_treeView_catalog->currentIndex());
        ui->u_tableView_filelist->setFocus();
    }
}

void ScanArchWindow::u_tableView_filelist_add()
{
    QString a0100 = archInfoHelper->getA0100();
    if (selectedCatalogInfo->LX == "1")
    {
        QString datafilepath_sm = FilesControl::GetLocalFilesPath_SM(a0100, selectedCatalogInfo->Code, selectedCatalogInfo->ID);
        qDebug() << "datafilepath_sm:" << datafilepath_sm << endl;
        QDir qdir(datafilepath_sm);
        if (!qdir.exists(datafilepath_sm))
        {
            if (!qdir.mkpath(datafilepath_sm))
            {
                return;
            }
        }
        QStringList filter;
        filter << "*.jpg";
        qdir.setNameFilters(filter);
        QFileInfoList fileInfoList = qdir.entryInfoList(filter);
        int lfileInfoListcount = fileInfoList.count();
        lfileInfoListcount++;
        QString filename = QString("%1").arg(lfileInfoListcount, 3, 10, QLatin1Char('0')) + ".jpg";
        QString allfilename = datafilepath_sm + '/' + filename;
        qDebug() << "allfilename is :" << allfilename << endl;
        u_frame_scan->PicWin->imageView->saveFile(allfilename);
        QStandardItem* qsItem1 = new QStandardItem(filename);
        QString sm_allfilepath = allfilename;
        qsItem1->setData(sm_allfilepath, Qt::UserRole + 1);
        qsimFilesList->setItem(lfileInfoListcount - 1, 0, qsItem1);
        QStandardItem* qsItem2 = new QStandardItem();
        QJsonObject  rqjo;
        QString ret = ControlUtils::findHasMd5E01ZB1(rqjo, selectedCatalogInfo->ID, filename) == true ? "是" : "否";
        qsItem2->setText(ret);
        qsimFilesList->setItem(lfileInfoListcount - 1, 1, qsItem2);
    }
}

void ScanArchWindow::on_btnClear_clicked()
{
    QString a0100 = archInfoHelper->getA0100();
    if (0 == QMessageBox::question(this, "提示", "您确定清空本卷图像吗?", "是", "否", "", 0, 1))
    {
        QString datafilepath_sm = FilesControl::GetLocalFilesPath_SM(a0100, "", "");
        QDir qdir(datafilepath_sm);
        if (qdir.exists(datafilepath_sm))
        {
            if (!qdir.removeRecursively())
            {
                return;
            }
        }
        on_u_treeView_catalog_clicked(ui->u_treeView_catalog->rootIndex());
    }
}

void ScanArchWindow::onControlTriggered()
{
    QString a0100 = archInfoHelper->getA0100();
    QAction* act = static_cast<QAction*>(sender());
    if (act->text() == "下载本类")
    {
        QArchScanGlobal::log("下载本类----1=");
        QModelIndex index = ui->u_treeView_catalog->currentIndex();
        QStandardItemModel* model = static_cast<QStandardItemModel*>(ui->u_treeView_catalog->model());
        QStandardItem* item = model->itemFromIndex(index);
        QList<DownLoadAndUpLoadThreadTaskInfo*> tasklist;
        ControlUtils::DownLoadDLFiles(item, tasklist, 1, a0100);
        QArchScanGlobal::log("下载本类----2=");
        if (tasklist.count() > 0)
        {
            QArchScanGlobal::log("下载本类----3=");
            ShowDownLoadAndUpLoadProgress(DownLoadAndUpLoadProgress::emDownLoad, tasklist);
        }
        qDeleteAll(tasklist);
    }
    else if (act->text() == "下载本件")
    {
        QModelIndex index = ui->u_treeView_catalog->currentIndex();
        QStandardItemModel* model = static_cast<QStandardItemModel*>(ui->u_treeView_catalog->model());
        QStandardItem* item = model->itemFromIndex(index);
        QList<DownLoadAndUpLoadThreadTaskInfo*> tasklist;
        ControlUtils::DownLoadXLFiles(item, tasklist, 1, a0100);
        if (tasklist.count() > 0)
        {
            ShowDownLoadAndUpLoadProgress(DownLoadAndUpLoadProgress::emDownLoad, tasklist);
        }
        qDeleteAll(tasklist);
    }
    else if (act->text() == "上传本类")
    {
        bool boolCoverServerPictureForUpload = true;
        QModelIndex index = ui->u_treeView_catalog->currentIndex();
        QStandardItemModel* model = static_cast<QStandardItemModel*>(ui->u_treeView_catalog->model());
        QStandardItem* item = model->itemFromIndex(index);
        QString lid = item->data(Qt::UserRole + 2).toString();
        QString lpid = item->data(Qt::UserRole + 3).toString();
        clearUploadCheckList();
        QJsonObject rjsonobj;
        ControlUtils::getDLCatalogData(rjsonobj, lid, lid, a0100);
        QList<QVariant> list = rjsonobj.value("data").toVariant().toList();
        if (!ControlUtils::checkUpload(qsimUploadCheckList, list, a0100))
        {
            ui->u_widget_uploadCheck->setVisible(true);
            uploadCheckSingleDirectorytableViewClicked("0"); //add fengjl 在下面显示修改信息区显示内容
            return;
        }
        QList<DownLoadAndUpLoadThreadTaskInfo*> tasklist;
        ControlUtils::UpLoadDLFiles(item, tasklist, 1, a0100);
        if (tasklist.count() > 0)
        {
            ShowDownLoadAndUpLoadProgress(DownLoadAndUpLoadProgress::emUpLoad, tasklist, boolCoverServerPictureForUpload);
        }
        qDeleteAll(tasklist);
    }
    else if (act->text() == "上传本件")
    {
        bool boolCoverServerPictureForUpload = true;
        //遍历目录
        QModelIndex index = ui->u_treeView_catalog->currentIndex();
        QStandardItemModel* model = static_cast<QStandardItemModel*>(ui->u_treeView_catalog->model());
        QStandardItem* item = model->itemFromIndex(index);
        QString lx = item->data(Qt::UserRole + 1).toString();
        QString lid = item->data(Qt::UserRole + 2).toString();
        if (lx == "0")
        {
            return;
        }
        clearUploadCheckList();
        QJsonObject rjsonobj;
        ControlUtils::getXLCatalogData(rjsonobj, lid, a0100);
        QList<QVariant> list = rjsonobj.value("data").toVariant().toList();
        if (!ControlUtils::checkUpload(qsimUploadCheckList, list, a0100))
        {
            ui->u_widget_uploadCheck->setVisible(true);
            uploadCheckSingleDirectorytableViewClicked("0"); //add fengjl 在下面显示修改信息区显示内容
            return;
        }
        QList<DownLoadAndUpLoadThreadTaskInfo*> tasklist;
        ControlUtils::UpLoadXLFiles(item, tasklist, 1, a0100);
        if (tasklist.count() > 0)
        {
            ShowDownLoadAndUpLoadProgress(DownLoadAndUpLoadProgress::emUpLoad, tasklist, boolCoverServerPictureForUpload);
        }
        qDeleteAll(tasklist);
    }
    else if (act->text() == "文件目录修改")
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
        bool ret = ControlUtils::EditArchInfo(item, id, this);
        if (ret)
        {
            emit refreshArchInfos();
        }
    }
    else if (act->text() == "上传本页")
    {
        bool boolCoverServerPictureForUpload = true;
        QModelIndex index = ui->u_tableView_filelist->currentIndex();
        if (index.row() < 0)
        {
            return;
        }
        QStandardItem* item = qsimFilesList->item(index.row());
        int nCount = selectedCatalogInfo->E01Z114;
        if (item != nullptr)
        {
            QString filename = item->text();
            int nPicXH = filename.mid(0, filename.indexOf(".")).toInt();
            if (nPicXH > nCount)
            {
                clearUploadCheckList();
                QJsonObject rjsonobj;
                ControlUtils::getXLCatalogData(rjsonobj, selectedCatalogInfo->ID, a0100);
                QList<QVariant> list = rjsonobj.value("data").toVariant().toList();
                if (!ControlUtils::checkUpload(qsimUploadCheckList, list, a0100))
                {
                    ui->u_widget_uploadCheck->setVisible(true);
                    uploadCheckSingleDirectorytableViewClicked("0");
                    return;
                }
            }

            QList<DownLoadAndUpLoadThreadTaskInfo*> tasklist;
            ControlUtils::UpLoadOneFile(item, selectedCatalogInfo, tasklist, 1, a0100);
            if (tasklist.count() > 0)
            {
                ShowDownLoadAndUpLoadProgress(DownLoadAndUpLoadProgress::emUpLoad, tasklist, boolCoverServerPictureForUpload);
            }
            qDeleteAll(tasklist);
        }
    }
    else if (act->text() == "下载本页")
    {
        QModelIndex filelistindex = ui->u_tableView_filelist->currentIndex();
        if (filelistindex.row() < 0)
        {
            return;
        }
        QStandardItem* filelistitem = qsimFilesList->item(filelistindex.row());
        QList<DownLoadAndUpLoadThreadTaskInfo*> tasklist;
        ControlUtils::DownLoadOneFile(filelistitem, tasklist, 1, a0100, selectedCatalogInfo->ID);
        if (tasklist.count() > 0)
        {
            ShowDownLoadAndUpLoadProgress(DownLoadAndUpLoadProgress::emDownLoad, tasklist);
        }
        qDeleteAll(tasklist);
    }
    else if (act->text() == "打开图片")
    {
        QModelIndex index = ui->u_tableView_filelist->currentIndex();
        QStandardItem* item = qsimFilesList->item(index.row());
        if (item == nullptr)
        {
            return;
        }
        QString path = item->data(Qt::UserRole + 1).toString();
        if (QFileInfo::exists(path))
        {
            QDesktopServices::openUrl(QUrl("file:///" + path));
        }
        else
        {
            QMessageBox::information(this, "提示", "图片不存在");
        }
    }
    else if (act->text() == "打开图片文件位置")
    {
        QModelIndex index = ui->u_tableView_filelist->currentIndex();
        QStandardItem* item = qsimFilesList->item(index.row());
        if (item == nullptr)
        {
            return;
        }
        QString path = item->data(Qt::UserRole + 1).toString();
        QFileInfo fileInfo(path);
        QDesktopServices::openUrl(QUrl("file:///" + fileInfo.absolutePath()));
    }
    else if (act->text() == "向上插入扫描")
    {
        QString lAllFilePath = qApp->applicationDirPath() + "/scantmp/" + QUuid::createUuid().toString(QUuid::WithoutBraces) + ".jpg";
#if QT_WIN
        if (ScannerAPP::m_ScannerCore->doScanToFile(lAllFilePath))
#else
        if (ScannerAPP::m_ScannerCore->doScanToFile(ScannerAPP::m_ScannerCore->scannerlist[m_ScannerConfig->sc_DefaultScan_Index]->get_saneHandle(), lAllFilePath))
#endif
        {
#ifdef QT_USE_UMSP_CryptCore
            CryptCore* m_cryptcore = CryptCore::GetCryptCore();
            if (!m_cryptcore->u_encryptFileSelf(lAllFilePath))
            {

                return;
            }
#endif
            //完成以后 需要更新旧文件名称
            QModelIndex index = ui->u_tableView_filelist->currentIndex();
            QStandardItem* item = qsimFilesList->item(index.row());
            int xh;
            if (item != nullptr)
            {
                xh = index.row() - 1;
            }
            else
            {
                xh = 0;
            }
            QString lDir = FilesControl::GetLocalFilesPath_SM(a0100, selectedCatalogInfo->Code, selectedCatalogInfo->ID);
            u_frame_scan->InitJPG(lAllFilePath);
            QFile file(lAllFilePath);
            if (file.exists())
            {
                file.remove();
            }
            QString lNewAllFilePath = QCoreApplication::applicationDirPath() + "/scantmp/tmp.jpg";
            u_frame_scan->PicWin->imageView->saveFile(lNewAllFilePath);
            ControlUtils::sortPicList(xh, lNewAllFilePath, lDir);
            on_u_treeView_catalog_clicked(ui->u_treeView_catalog->currentIndex());
            if (xh < 0)
            {
                xh = 0;
            }
            QStandardItem* itemchild = qsimFilesList->item(xh);
            if (item == nullptr)
            {
                return;
            }
            ui->u_tableView_filelist->setCurrentIndex(itemchild->index());
            on_u_tableView_filelist_clicked(ui->u_tableView_filelist->currentIndex());
        }
        else
        {
            QArchScanGlobal::information(this, this->windowTitle(), "扫描失败，请重试");
        }
    }
    else if (act->text() == "向下插入扫描")
    {
        QString lAllFilePath = qApp->applicationDirPath() + "/scantmp/" + QUuid::createUuid().toString(QUuid::WithoutBraces) + ".jpg";
#if QT_WIN
        if (ScannerAPP::m_ScannerCore->doScanToFile(lAllFilePath))
#else
        if (ScannerAPP::m_ScannerCore->doScanToFile(ScannerAPP::m_ScannerCore->scannerlist[m_ScannerConfig->sc_DefaultScan_Index]->get_saneHandle(), lAllFilePath))
#endif
        {
#ifdef QT_USE_UMSP_CryptCore
            CryptCore* m_cryptcore = CryptCore::GetCryptCore();
            if (!m_cryptcore->u_encryptFileSelf(lAllFilePath))
            {

                return;
            }
#endif
            u_frame_scan->InitJPG(lAllFilePath);//显示新插入的文件
            //完成以后 需要更新旧文件名称
            QModelIndex index = ui->u_tableView_filelist->currentIndex();
            QStandardItem* item = qsimFilesList->item(index.row());
            int xh;
            if (item != nullptr)
            {
                xh = index.row() + 1;//新文件的序号
            }
            else
            {
                xh = 0;
            }
            QString lDir = FilesControl::GetLocalFilesPath_SM(a0100, selectedCatalogInfo->Code, selectedCatalogInfo->ID);
            QFile file(lAllFilePath);
            if (file.exists())
            {
                file.remove();
            }//删除扫描的文件
            QString lNewAllFilePath = QCoreApplication::applicationDirPath() + "/scantmp/tmp.jpg";//新的文件名
            u_frame_scan->PicWin->imageView->saveFile(lNewAllFilePath);
            u_frame_scan->InitJPG(lNewAllFilePath);
            ControlUtils::sortPicList(xh, lNewAllFilePath, lDir);//排序
            on_u_treeView_catalog_clicked(ui->u_treeView_catalog->currentIndex());//刷新列表
            QStandardItem* itemchild = qsimFilesList->item(xh);
            if (item == nullptr)
            {
                return;
            }
            ui->u_tableView_filelist->setCurrentIndex(itemchild->index());
            on_u_tableView_filelist_clicked(ui->u_tableView_filelist->currentIndex());
        }
        else
        {
            QArchScanGlobal::information(this, this->windowTitle(), "扫描失败，请重试");
        }
    }
    else if (act->text() == "上移")
    {
        //图片重命名
        QModelIndex index = ui->u_tableView_filelist->currentIndex();
        QStandardItem* item1 = qsimFilesList->item(index.row());
        if (item1 == nullptr)
        {
            return;
        }
        QString path1 = item1->data(Qt::UserRole + 1).toString();
        QStandardItem* item2 = qsimFilesList->item(index.row() - 1);
        if (item2 == nullptr)
        {
            return;
        }
        QString path2 = item2->data(Qt::UserRole + 1).toString();
        ControlUtils::exchangePic(path1, path2);
        on_btnPrePage_clicked();
    }
    else if (act->text() == "下移")
    {
        //图片重命名
        QModelIndex index = ui->u_tableView_filelist->currentIndex();
        QStandardItem* item1 = qsimFilesList->item(index.row());
        if (item1 == nullptr)
        {
            return;
        }
        QString path1 = item1->data(Qt::UserRole + 1).toString();
        QStandardItem* item2 = qsimFilesList->item(index.row() + 1);
        if (item2 == nullptr)
        {
            return;
        }
        QString path2 = item2->data(Qt::UserRole + 1).toString();
        ControlUtils::exchangePic(path1, path2);
        on_btnNextPage_clicked();
    }
    else if (act->text() == "删除文件")
    {
        delete_u_tableView_filelist_currentitem();
    }
    else if (act->text() == "清空本件图像")
    {
        QStandardItemModel* model = static_cast<QStandardItemModel*>(ui->u_treeView_catalog->model());
        QStandardItem* item = model->itemFromIndex(ui->u_treeView_catalog->currentIndex());
        if (!item)
        {
            return;
        }
        QString lx = item->data(Qt::UserRole + 1).toString();
        QString lid = item->data(Qt::UserRole + 2).toString();
        if (lx == "0")
        {
            return;
        }
        if (0 == QMessageBox::question(this, "提示", "您确定清空本件图像吗?", "是", "否", "", 0, 1))
        {
            QString datafilepath_sm = FilesControl::GetLocalFilesPath_SM(a0100, selectedCatalogInfo->Code, selectedCatalogInfo->ID);
            qDebug() << "datafilepath_sm:" << datafilepath_sm << endl;
            QDir qdir(datafilepath_sm);
            if (qdir.exists(datafilepath_sm))
            {
                if (!qdir.removeRecursively())
                {
                    return;
                }
            }
            on_u_treeView_catalog_clicked(ui->u_treeView_catalog->currentIndex());
        }
    }
    else if (act->text() == "清空本类图像")
    {
        if (0 != QMessageBox::question(this, "提示", "您确定清空本类图像吗?", "是", "否", "", 0, 1))
        {
            return;
        }
        QString lid2 = "";
        QModelIndex catalog_row2 = ui->u_treeView_catalog->currentIndex().siblingAtRow(ui->u_treeView_catalog->currentIndex().row() + 1);
        if (catalog_row2.isValid())
        {
            QStandardItem* item2 = qsimCatalog->itemFromIndex(catalog_row2);
            if (item2)
            {
                lid2 = item2->data(Qt::UserRole + 2).toString();
            }
        }
        QModelIndex catalog_indexBelow = ui->u_treeView_catalog->indexBelow(ui->u_treeView_catalog->currentIndex());
        while (catalog_indexBelow.isValid())
        {
            QStandardItem* item = qsimCatalog->itemFromIndex(catalog_indexBelow);
            if (item != nullptr)
            {
                QString lx = item->data(Qt::UserRole + 1).toString();
                QString lid = item->data(Qt::UserRole + 2).toString();
                if (lid == lid2)
                {
                    break;
                }
                if (lx == "1")
                {
                    QString datafilepath_sm = FilesControl::GetLocalFilesPath_SM(a0100, lid, "");
                    QArchScanGlobal::clearFolder(datafilepath_sm);
                }
                catalog_indexBelow = ui->u_treeView_catalog->indexBelow(item->index());
            }
            else
            {
                break;
            }
        }
        on_u_treeView_catalog_clicked(ui->u_treeView_catalog->currentIndex());
    }
    else if (act->text() == "添加图片")
    {
        QFileDialog* pFileDlg = new QFileDialog(this);
        pFileDlg->setWindowTitle(tr("Add File"));
        pFileDlg->setAcceptMode(QFileDialog::AcceptOpen);
        pFileDlg->setFileMode(QFileDialog::ExistingFiles);
        pFileDlg->setOption(QFileDialog::ReadOnly);
        pFileDlg->setDirectory(".");
        QStringList filters;
        //filters << tr("Image Files(*.pdf *.png *.bmp *.jpg *.jpeg *.tif *.tiff)");
        filters << tr("Image Files(*.jpg)");
        pFileDlg->setNameFilters(filters);
        if (pFileDlg->exec() == QDialog::Accepted)
        {
            QStringList filePaths;
            filePaths = pFileDlg->selectedFiles();
            //检测300DPI
            QImage qImage;
            foreach (QString srcFilePath, filePaths)
            {
                qImage.load(srcFilePath);
                if (qRound(qImage.dotsPerMeterX() * 0.0254) < 300.00 || qRound(qImage.dotsPerMeterY() * 0.0254) < 300.00)
                {
                    QArchScanGlobal::information(nullptr, "提示", "图片DPI小于300,不合规！ " + srcFilePath);
                    QArchScanGlobal::information(nullptr, "提示", "中断添加图片,请检查图片！");
                    return;
                }
            }
            foreach (QString srcFilePath, filePaths)
            {
                QString sTmpFilePath = qApp->applicationDirPath() + "/scantmp/" + QUuid::createUuid().toString(QUuid::WithoutBraces) + ".jpg";
                if (!QFile::copy(srcFilePath, sTmpFilePath))
                {
                    qDebug() << "Copy file from " << srcFilePath << " TO " << sTmpFilePath << " failed";
                    continue;
                }
#ifdef QT_USE_UMSP_CryptCore
                CryptCore* m_cryptcore = CryptCore::GetCryptCore();
                if (!m_cryptcore->u_encryptFileSelf(sTmpFilePath))
                {
                    return;
                }
#endif
                AddTwainJpg(sTmpFilePath);
            }
        }
    }
}

void ScanArchWindow::on_u_treeView_catalog_customContextMenuRequested(const QPoint& pos)
{
    Q_UNUSED(pos)
    QModelIndex index = ui->u_treeView_catalog->currentIndex();
    QStandardItemModel* model = static_cast<QStandardItemModel*>(ui->u_treeView_catalog->model());
    QStandardItem* item = model->itemFromIndex(index);
    QString lx = item->data(Qt::UserRole + 1).toString();
    QMenu* cmenu = new QMenu(ui->u_treeView_catalog);
    if (lx == "0")
    {
        QString actionNames[3] = { "下载本类", "上传本类", "清空本件图像" };
        for (int i = 0; i < 3; i++)
        {
            QAction* act = cmenu->addAction(actionNames[i]);
            connect(act, SIGNAL(triggered()), this, SLOT(onControlTriggered()));
        }
    }
    else if (lx == "1")
    {
        QString actionNames[4] = { "下载本件", "上传本件", "清空本件图像", "文件目录修改" };
        for (int i = 0; i < 4; i++)
        {
            QAction* act = cmenu->addAction(actionNames[i]);
            connect(act, SIGNAL(triggered()), this, SLOT(onControlTriggered()));
        }
    }
    cmenu->exec(QCursor::pos());
}

void ScanArchWindow::on_u_tableView_filelist_customContextMenuRequested(const QPoint& pos)
{
    Q_UNUSED(pos)
    QModelIndex filelistindex = ui->u_tableView_filelist->currentIndex();
    QMenu* cmenu = new QMenu(ui->u_treeView_catalog);
    if (filelistindex.row() < 0)
    {
        QAction* action1 = cmenu->addAction("下载本件");
        QAction* action2 = cmenu->addAction("添加图片");
        connect(action1, SIGNAL(triggered()), this, SLOT(onControlTriggered()));
        connect(action2, SIGNAL(triggered()), this, SLOT(onControlTriggered()));
    }
    else
    {
        QStandardItem* filelistitem = qsimFilesList->item(filelistindex.row());
        if (filelistitem != nullptr)
        {
            QString actionNames[12] = { "向上插入扫描", "向下插入扫描", "下载本页",
                                        "上传本页", "上移", "下移",
                                        "下载本件", "上传本件", "打开图片", "打开图片文件位置",
                                        "删除文件", "添加图片"
                                      };
            for (int i = 0; i < 12; i++)
            {
                QAction* act = cmenu->addAction(actionNames[i]);
                connect(act, SIGNAL(triggered()), this, SLOT(onControlTriggered()));
            }
        }
    }
    cmenu->exec(QCursor::pos());
}

void ScanArchWindow::on_u_pushButton_uploadCheckListShow_clicked()
{
    if (ui->u_widget_uploadCheck->isVisible())
    {
        ui->u_widget_uploadCheck->setVisible(false);
    }
    else
    {
        refreshCheckList();
    }
}

void ScanArchWindow::update_u_groupBox_uploadCheck_button(int astate)
{
    switch (astate)
    {
    case 0:
        setControlEnable(false, false, false, false);
        break;
    case 1:
        setControlEnable(true, false, false, false);
        break;
    case 2:
        ui->u_spinBox_totalpage->selectAll();
        setControlEnable(false, true, true, true);
        break;
    default:
        setControlEnable(false, false, false, false);
    }
}

void ScanArchWindow::on_u_pushButton_editUploadCheck_clicked()
{
    update_u_groupBox_uploadCheck_button(2);
}

void ScanArchWindow::on_u_pushButton_cancelUploadCheck_clicked()
{
    QStandardItem* lItem = qsimUploadCheckList->item(ui->u_tableView_uploadCheck->currentIndex().row(), 0);
    if (lItem != nullptr)
    {
        setSpinBoxValues(lItem);
    }
    update_u_groupBox_uploadCheck_button(lItem == nullptr ? 0 : 1);
}

void ScanArchWindow::on_u_pushButton_saveUploadCheck_clicked()
{
    int totolpage = ui->u_spinBox_totalpage->value();
    QStandardItem* lItem = qsimUploadCheckList->item(ui->u_tableView_uploadCheck->currentIndex().row(), 0);
    if (!lItem)
    {
        lItem = qsimUploadCheckList->item(0); //增加判断及设置默认值
    }
    if (lItem != nullptr)
    {
        QString lId = lItem->data(Qt::UserRole + 1).toString();
        int scanedpagecount = lItem->data(Qt::UserRole + 3).toInt();
        QJsonObject rjsonobj;
        if (ControlUtils::updateE01Z1AndDeleteE01ZB1(rjsonobj, lId, QString::number(totolpage)))
        {
            int lchazhi = totolpage - scanedpagecount;
            lItem->setData(totolpage, Qt::UserRole + 2);
            if (lchazhi == 0)
            {
                lItem->setData(0, Qt::UserRole + 4);
                lItem->setData(0, Qt::UserRole + 5);
            }
            else if (lchazhi > 0)
            {
                lItem->setData(abs(lchazhi), Qt::UserRole + 4);
                lItem->setData(0, Qt::UserRole + 5);
            }
            else if (lchazhi < 0)
            {
                lItem->setData(0, Qt::UserRole + 4);
                lItem->setData(abs(lchazhi), Qt::UserRole + 5);
            }
            setSpinBoxValues(lItem);
            update_u_groupBox_uploadCheck_button(1);

            //更新数据
            QString lId = lItem->data(Qt::UserRole + 1).toString();
            QStandardItem* catelogItem = ControlUtils::GetItemFromqsimCatalog(qsimCatalog, lId);
            if (catelogItem != nullptr)
            {
                catelogItem->setData(ui->u_spinBox_totalpage->value(), Qt::UserRole + 5);
                emit ui->u_treeView_catalog->clicked(ui->u_treeView_catalog->currentIndex());
                refreshCheckList();
            }

        }
        else
        {
            QArchScanGlobal::information(nullptr, "提示", "修改数量失败!");
        }
    }
}

void ScanArchWindow::clearUploadCheckList()
{
    qsimUploadCheckList->removeRows(0, qsimUploadCheckList->rowCount());
    qsimUploadCheckList->clear();
    qsimUploadCheckList->setColumnCount(2);
    qsimUploadCheckList->setHeaderData(0, Qt::Horizontal, "材料名称");
    qsimUploadCheckList->setHeaderData(1, Qt::Horizontal, "结果");
    setSpinBoxValues(nullptr);
    update_u_groupBox_uploadCheck_button(0);
}

void ScanArchWindow::keyPressEvent(QKeyEvent* event)
{
    switch (event->key())
    {
    case Qt::Key_F3:
    {
#ifdef QT_WIN
        if (!ScannerAPP::m_ScannerCore->setStandardSize(TWSS_A3))
        {
            return;
        }
#else
        SANE_String lSANE_String = "A3";
        if (!ScannerAPP::m_ScannerCore->set_papersize_option(ScannerAPP::m_ScannerCore->scannerlist[m_ScannerConfig->sc_DefaultScan_Index]->get_saneHandle(), \
                                                             ScannerAPP::m_ScannerCore->scannerlist[m_ScannerConfig->sc_DefaultScan_Index]->get_color_optionID(), lSANE_String))
        {
            return;
        }
        m_ScannerConfig->sc_StandardSize = 0;
#endif
        m_ScannerConfig->sc_StandardSize = 0;
        m_ScannerConfig->updateUI();
        m_ScannerConfig->savConfig();
        break;
    }
    case Qt::Key_F4:
    {
#ifdef QT_WIN
        if (!ScannerAPP::m_ScannerCore->setStandardSize(TWSS_A4))
        {
            return;
        }
#else
        SANE_String lSANE_String = "A4";
        if (!ScannerAPP::m_ScannerCore->set_papersize_option(ScannerAPP::m_ScannerCore->scannerlist[m_ScannerConfig->sc_DefaultScan_Index]->get_saneHandle(), \
                                                             ScannerAPP::m_ScannerCore->scannerlist[m_ScannerConfig->sc_DefaultScan_Index]->get_color_optionID(), lSANE_String))
        {
            return;
        }
        m_ScannerConfig->sc_StandardSize = 1;
#endif
        m_ScannerConfig->sc_StandardSize = 1;
        m_ScannerConfig->updateUI();
        m_ScannerConfig->savConfig();
        break;
    }
    case Qt::Key_F7:
    {
        sl_on_u_pushButton_scan_clicked();
        break;
    }
    case Qt::Key_Delete:
    {
        delete_u_tableView_filelist_currentitem();
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

void ScanArchWindow::uploadCheckSingleDirectory(QString id) //id 为E01Z100
{
    Q_UNUSED(id)
    QString lTiShi = "提示: 应扫【%1】页, 实扫【%2】页。";
    QString a0100 = archInfoHelper->getA0100();
    QString larg1 = QString::number(selectedCatalogInfo->E01Z114);
    QString larg2 = "0";
    QString larg3 = "";
    QString datafilepath_sm = FilesControl::GetLocalFilesPath_SM(a0100, selectedCatalogInfo->Code, selectedCatalogInfo->ID);
    QDir qdir(datafilepath_sm);
    if (qdir.exists(datafilepath_sm))
    {
        QStringList filter;
        filter << "*.jpg";
        qdir.setNameFilters(filter);
        larg2 = QString::number(qdir.entryList().count());
    }
    lTiShi = lTiShi.arg(larg1, larg2);
    ui->u_label_statusbarTiShi->setText(lTiShi);
}

void ScanArchWindow::uploadCheckSingleDirectorytableViewClicked(QString lId)
{
    Q_UNUSED(lId)
    QStandardItem* lItem = qsimUploadCheckList->item(0);
    if (lItem != nullptr)
    {
        update_u_groupBox_uploadCheck_button(1);
        QString lId = lItem->data(Qt::UserRole + 1).toString();
        QStandardItem* catelogItem = ControlUtils::GetItemFromqsimCatalog(qsimCatalog, lId);
        if (catelogItem != nullptr)
        {
            ui->u_treeView_catalog->setCurrentIndex(catelogItem->index());
            setSpinBoxValues(lItem);
            update_u_groupBox_uploadCheck_button(2); //设置可修改状态
        }
    }
    else
    {
        update_u_groupBox_uploadCheck_button(0);
    }
}

void ScanArchWindow::on_u_tableView_uploadCheck_clicked(const QModelIndex& index)
{
    QStandardItem* lItem = qsimUploadCheckList->item(index.row(), 0);
    if (lItem != nullptr)
    {
        update_u_groupBox_uploadCheck_button(1);
        QString lId = lItem->data(Qt::UserRole + 1).toString();
        QStandardItem* catelogItem = ControlUtils::GetItemFromqsimCatalog(qsimCatalog, lId);
        if (catelogItem != nullptr)
        {
            ui->u_treeView_catalog->setCurrentIndex(catelogItem->index());
            on_u_treeView_catalog_clicked(ui->u_treeView_catalog->currentIndex());
            setSpinBoxValues(lItem);
            update_u_groupBox_uploadCheck_button(2); //设置可修改状态
        }
    }
    else
    {
        update_u_groupBox_uploadCheck_button(0);
    }
}

void ScanArchWindow::on_u_pushButton_upLoadALL_clicked()
{
    //上传本卷图像
    UpLoadFile();
}

void ScanArchWindow::on_u_pushButton_downLoadALL_clicked()
{
    //下载本卷图像
    FileDownloader* filedownLoader = new FileDownloader(this);
    filedownLoader->downloadAll(this, ui->u_treeView_catalog, archInfoHelper->getA0100(), 1);
}

void ScanArchWindow::delete_u_tableView_filelist_currentitem()
{
    QItemSelectionModel* model_selection = ui->u_tableView_filelist->selectionModel();
    QModelIndexList IndexList = model_selection->selectedIndexes();
    if (IndexList.count() == 0)
    {
        return;
    }
    else
    {
        if (QMessageBox::question(this, "提示", "确认要删除选中的文件吗？", "是", "否") != 0)
        {
            return;
        }
    }
    QString ys_allfilepath22;
    QString dirPicturePath;
    //1、删除图片
    foreach (QModelIndex index, IndexList)
    {
        if (! index.isValid())
        {
            return;
        }
        if (index.column() != 0)
        {
            continue;
        }
        QStandardItem* item = qsimFilesList->item(index.row());
        if (item != nullptr)
        {
            ys_allfilepath22 = item->data(Qt::UserRole + 1).toString();
            QFile deletefile(ys_allfilepath22);
            if (deletefile.exists())
            {
                deletefile.remove();
            }
        }
    }
    //2、重命名剩下的图片
    dirPicturePath = QArchScanGlobal::getFileAbsolutePath(ys_allfilepath22);
    QDir qdir(dirPicturePath);
    QString names;
    if (qdir.exists(dirPicturePath))
    {
        QStringList filter;
        filter << "*.jpg";
        //       QStringList filelist;
        int xh = 0;
        foreach (QString file, qdir.entryList(filter, QDir::Files))
        {
            xh++;
            QString filename = QString("%1").arg(xh, 3, 10, QLatin1Char('0')) + ".jpg";
            QString oldFileAllPath = dirPicturePath + '/' + file;
            ControlUtils::RenamePic(oldFileAllPath, filename);
        }
    }
    //3、刷新当前目录的图片
    QModelIndex qmindex = ui->u_treeView_catalog->currentIndex();
    on_u_treeView_catalog_clicked(qmindex);
}

void ScanArchWindow::sl_treeView_catalog_dropEvent(QModelIndex droptoindex)
{
    QStandardItem* qsimCatalogitem = qsimCatalog->itemFromIndex(droptoindex);
    QString a0100 = archInfoHelper->getA0100();
    if (qsimCatalogitem)//pInfo
    {
        QString CatalogInfo_LX = qsimCatalogitem->data(Qt::UserRole + 1).toString();//pInfo->m_LX;
        QString CatalogInfo_ID = qsimCatalogitem->data(Qt::UserRole + 2).toString();//pInfo->m_ID;
        QString CatalogInfo_PID = qsimCatalogitem->data(Qt::UserRole + 3).toString();//pInfo->m_PID;
        QString CatalogInfo_SXH = qsimCatalogitem->data(Qt::UserRole + 4).toString();//pInfo->m_SXH;
        //int CatalogInfo_E01Z114 = pInfo->m_E01Z114;
        QString CatalogInfo_Code = qsimCatalogitem->data(Qt::UserRole + 6).toString();//pInfo->m_Code;
        if (CatalogInfo_LX != "1")
        {
            return;
        }
        //待处理的图片集合
        QItemSelectionModel* model_selection = ui->u_tableView_filelist->selectionModel();
        QModelIndexList IndexList = model_selection->selectedIndexes();
        if (IndexList.count() == 0)
        {
            return;
        }
        //目标目录的原始图片路径
        QString datafilepath_ys = FilesControl::GetLocalFilesPath_YS(a0100, CatalogInfo_Code, CatalogInfo_ID);
        if (!QArchScanGlobal::isdirexists(datafilepath_ys))
        {
            if (!QArchScanGlobal::dircreate(datafilepath_ys))
            {
                return;
            }
        }
        //目标目录的最大编号
        int ifileInfoListcount = QArchScanGlobal::getMaxCountFromDefaultJPGFilePath(datafilepath_ys);
        ifileInfoListcount++;
        //1、移动图片
        QString ys_allfilepath_source;
        foreach (QModelIndex index, IndexList)
        {
            if (! index.isValid())
            {
                return;
            }
            if (index.column() != 0)
            {
                continue;
            }
            QStandardItem* item = qsimFilesList->item(index.row());
            if (item != nullptr)
            {
                ys_allfilepath_source = item->data(Qt::UserRole + 1).toString();
                QString filename = QString("%1").arg(ifileInfoListcount, 3, 10, QLatin1Char('0')) + ".jpg";
                QString allfilename = datafilepath_ys + '/' + filename;
                if (!QFile::copy(ys_allfilepath_source, allfilename))
                {
                    return;
                }
                QFile deletefile(ys_allfilepath_source);
                if (deletefile.exists())
                {
                    deletefile.remove();
                }
            }
            ifileInfoListcount++;
        }
        //2、重命名原始目录的剩下图片
        QString dirPicturePath;
        dirPicturePath = QArchScanGlobal::getFileAbsolutePath(ys_allfilepath_source);
        QDir qdir(dirPicturePath);
        QString names;
        if (qdir.exists(dirPicturePath))
        {
            QStringList filter;
            filter << "*.jpg";
            int xh = 0;
            foreach (QString file, qdir.entryList(filter, QDir::Files))
            {
                xh++;
                QString filename = QString("%1").arg(xh, 3, 10, QLatin1Char('0')) + ".jpg";
                QString oldFileAllPath = dirPicturePath + '/' + file;
                ControlUtils::RenamePic(oldFileAllPath, filename);
            }
        }
        //3、刷新当前目录(来源)的图片
        QModelIndex qmindex = ui->u_treeView_catalog->currentIndex();
        on_u_treeView_catalog_clicked(qmindex);
    }
}

void ScanArchWindow::on_u_tableView_filelist_pressed(const QModelIndex& index)
{
    tableView_filelist_pressedQModelIndex = index;
}

void ScanArchWindow::resizeEvent(QResizeEvent* event)
{
    Q_UNUSED(event)
    searchPersonInfoExForm->setGeometry(0, 0, searchPersonInfoExForm->getCurrentWidth(),
                                        ui->u_widget_client->height());
    ui->wgtImgClient->setGeometry(searchPersonInfoExForm->getCurrentWidth(), 0,
                                  ui->u_widget_client->width() - searchPersonInfoExForm->getCurrentWidth(),
                                  ui->u_widget_client->height());
}

void ScanArchWindow::onResize()
{
    resizeEvent(nullptr);
}

void ScanArchWindow::onSelectPerson(QTableView* tableView, QString id, QString xm, QString cardNum)
{
    Q_UNUSED(tableView)
    archInfoHelper->loadData(id);
    QString lwindowTitle = "档案扫描 姓名: %1  身份证号:%2";
    lwindowTitle = lwindowTitle.arg(xm).arg(cardNum);
    setWindowTitle(lwindowTitle);
}

bool ScanArchWindow::eventFilter(QObject* obj, QEvent* e)
{
    Q_UNUSED(obj)
    if (e->type() == QEvent::KeyPress)
    {
        this->keyPressEvent(static_cast<QKeyEvent*>(e));
        if (static_cast<QKeyEvent*>(e)->key() == Qt::Key_F7 || static_cast<QKeyEvent*>(e)->key() == Qt::Key_F3
            || static_cast<QKeyEvent*>(e)->key() == Qt::Key_F4 || static_cast<QKeyEvent*>(e)->key() == Qt::Key_Delete ||
            static_cast<QKeyEvent*>(e)->key() == Qt::Key_Space)
        {
            return true;
        }
    }
    return false;
}

//void ScanArchWindow::on_u_treeView_catalog_currentChanged(const QModelIndex& current, const QModelIndex& previous)
//{
//    Q_UNUSED(previous)
//    on_u_treeView_catalog_clicked(current);
//}

void ScanArchWindow::on_u_tableView_filelist_currentChanged(const QModelIndex& current, const QModelIndex& previous)
{
    if (current.row() != previous.row())
    {
        on_u_tableView_filelist_clicked(current);
    }
}

bool ScanArchWindow::bTClickLessthan(uint abctime)
{
    QDateTime lnowtime = QDateTime::currentDateTime();
    uint lnowtimeT = lnowtime.toTime_t(); //获取当前时间
    bool rets = (lnowtimeT - m_lastclickedT) <= abctime;
    m_lastclickedtime = lnowtime;
    m_lastclickedT = lnowtimeT;
    return rets;
}

void ScanArchWindow::onSrcImageViewChangeOperateType(ImageView::ImageOperateType operateType)
{
    ui->label_2n->setText("原始图像数据      当前操作：[" + u_frame_scan->PicWin->imageView->getOperatorTypeName(operateType) + "]");
}


///////////////////////////////////////////////////////////////////////////////////////////

void ScannerAPP::InitAPP()
{
    if (m_ScannerCore != nullptr)
    {
        return;
    }
#ifdef QT_WIN
    ScannerAPP::m_ScannerCore = DTwainAPP::getDTInstance();
#else
    ScannerAPP::m_ScannerCore = Scanner_Operation::getInstance();
    m_ScannerCore->Sane_init();
#endif
}

void ScanArchWindow::refreshCheckList()
{
    if (!ui->u_widget_uploadCheck->isVisible())
    {
        ui->u_widget_uploadCheck->setVisible(true);
    }
    QJsonObject  rqjo;
    if (ControlUtils::selectAllArchiveCatalogDetail(rqjo, archInfoHelper->getA0100()))
    {
        QJsonArray jsonarry = rqjo.value("data").toArray();
        if (!ControlUtils::checkAllUpload(qsimUploadCheckList, jsonarry, archInfoHelper->getA0100()))
        {}
    }
    int n = ui->u_tableView_uploadCheck->currentIndex().row() >= 0 ? 1 : 0;
    update_u_groupBox_uploadCheck_button(n);
}

void ScanArchWindow::showMessage(int type, QString title, QString content)
{
    if (type == 0)
    {
        QArchScanGlobal::information(nullptr, title, content);
    }
}

void ScanArchWindow::on_u_checkBox_checkCanScanNode_clicked()
{
    auto_split = ui->u_checkBox_checkCanScanNode->isChecked();
    qDebug() << "auto_split:" << auto_split << endl;
    save_settting();
}

void ScanArchWindow::on_u_checkBox_AutoScan_clicked()
{
    auto_scan = ui->u_checkBox_AutoScan->isChecked();
    qDebug() << "auto_scan: " << auto_scan << endl;
    save_settting();
}

void ScanArchWindow::on_u_doubleSpinBox_AutoScanM_editingFinished()
{
    interval_time = QString::number(ui->u_doubleSpinBox_AutoScanM->value(), 'f', 2);
    qDebug() << "interval_time:" << interval_time  << endl;
    save_settting();
}
