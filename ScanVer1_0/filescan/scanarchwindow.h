/**
* @projectName   qtarchivesscanhandling
* @brief         扫描模块
* @author
* @copyright     北京航星永志科技有限公司
* @date          2019-11-26
*/
#ifndef SCANARCHWINDOW_H
#define SCANARCHWINDOW_H

#include <QMainWindow>
#include <QModelIndex>
#include <QDateTime>
#include <QUuid>
#include <QMutex>
#include "src/control/imagecontrol/imageview.h"


namespace Ui
{
class ScanArchWindow;
}

class ArchInfoHelper;
class QTableView;
class SearchPersonInfoExForm;
class DownLoadAndUpLoadProgress;
class QStandardItem;
class OcrcollectForm;
class ScanPictureProcessWindow;
class QStandardItemModel;
class TSelectedCatalogInfo;
class DownLoadAndUpLoadThreadTaskInfo;
class ScannerConfig;
class ScannerConfig_Sane;
class DTwainAPP;
class Scanner_Operation;
class ScanArchWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit ScanArchWindow(QWidget* parent = nullptr);
    ~ScanArchWindow();

    void setSpinBoxValues(QStandardItem* item);
    void setControlEnable(bool b0, bool b1, bool b2, bool b3);
    void setLoadedFinish(bool bFinished);
protected:
    virtual void closeEvent(QCloseEvent* event);
signals:
    void refreshArchInfos();
private:
    //    显示上传下载进度窗口
    void ShowDownLoadAndUpLoadProgress(int type, QList<DownLoadAndUpLoadThreadTaskInfo*> tasklist, bool aboolCoverServerPictureForUpload = false);
    //    上传本卷
    void UpLoadFile();
    //添加图片文件
    void u_tableView_filelist_add();
    //插入扫描图片
    void AddTwainJpg(QString AllFilePath);
    //更新校验结果列表 0=初始 1修改状态
    void update_u_groupBox_uploadCheck_button(int astate);
    //清空上传列表
    void clearUploadCheckList();
    // 验证tableView的点击事件
    void uploadCheckSingleDirectorytableViewClicked(QString lId);

    // 删除图片列表中的当前选择的图片
    void delete_u_tableView_filelist_currentitem();
    bool eventFilter(QObject* obj, QEvent* e);
//    void on_u_treeView_catalog_currentChanged(const QModelIndex& current, const QModelIndex& previous);
    void on_u_tableView_filelist_currentChanged(const QModelIndex& current, const QModelIndex& previous);
    //两次点击时间间隔小于
    bool bTClickLessthan(uint uabctime = 1000);
    //高速扫描
    void doScanToFileList();
    //平板扫描
    void doScanToFile();
    void refreshCheckList();
    //高扫后的图片更新显示当前UI
    void UpdateShowUIForHighScan(QString AllFilePath);

    void setting_init();
    void save_settting();
    void check_ini();

private slots:
    void sl_DownLoadAndUpLoadProgressOnClose();
    void showMessage(int type, QString title, QString content);
    //===========================工具栏按钮=================================
    // 设置扫描仪
    void on_btnScanner_clicked();
    // 打开关闭校验列表
    void on_u_pushButton_uploadCheckListShow_clicked();
    // 上传本卷
    void on_u_pushButton_upLoadALL_clicked();
    // 下载本卷
    void on_u_pushButton_downLoadALL_clicked();
    // 清空本卷图像
    void on_btnClear_clicked();
    // 上一页
    void on_btnPrePage_clicked();
    // 下一页
    void on_btnNextPage_clicked();
    //下一目录
    void on_btnNextCata_clicked();
    //===================================================================

    // 十大类目录树被点击
    void on_u_treeView_catalog_clicked(const QModelIndex& index);
    // 图片列表被点击
    void on_u_tableView_filelist_clicked(const QModelIndex& index);
    // 扫描按钮响应
    void sl_on_u_pushButton_scan_clicked();
    // 右键菜单事件合集
    void onControlTriggered();
    // 自定义右键,ui中需要响应属性配合
    void on_u_treeView_catalog_customContextMenuRequested(const QPoint& pos);
    void on_u_tableView_filelist_customContextMenuRequested(const QPoint& pos);
    //校验列表 窗口的tableView点击
    void on_u_tableView_uploadCheck_clicked(const QModelIndex& index);
    void on_u_pushButton_editUploadCheck_clicked();
    void on_u_pushButton_cancelUploadCheck_clicked();
    void on_u_pushButton_saveUploadCheck_clicked();
    //快捷键
    void keyPressEvent(QKeyEvent* event);
    //点击左边的目录，检验显示时，触发检验检查
    void uploadCheckSingleDirectory(QString id);
    //目录拖拽处理事件
    void sl_treeView_catalog_dropEvent(QModelIndex droptoindex);
    void on_u_tableView_filelist_pressed(const QModelIndex& index);
    void resizeEvent(QResizeEvent* event);
    void onResize();
    void onSelectPerson(QTableView* tableView, QString id, QString xm, QString cardNum);
    void onSrcImageViewChangeOperateType(ImageView::ImageOperateType operateType);

    void on_u_checkBox_checkCanScanNode_clicked();

    void on_u_checkBox_AutoScan_clicked();

    void on_u_doubleSpinBox_AutoScanM_editingFinished();

public:
    SearchPersonInfoExForm* searchPersonInfoExForm;
    ///加载目录的辅助类
    ArchInfoHelper* archInfoHelper;
    static void CallBackFunShowOnePicFromScannerFrm(int FileIndex);

private:
    Ui::ScanArchWindow* ui;
    DownLoadAndUpLoadProgress* winDownUpProgress = nullptr;
    OcrcollectForm* ocrcollectform = nullptr;
    ScanPictureProcessWindow* u_frame_scan;
    QStandardItemModel* qsimCatalog;
    QStandardItemModel* qsimFilesList;
    QStandardItemModel* qsimUploadCheckList;
    QString m_A0100;
    TSelectedCatalogInfo* selectedCatalogInfo;
    //自动扫描定时器
    QTimer* m_ScanTimer;
#ifdef QT_WIN
    ScannerConfig* m_ScannerConfig;
#else
    ScannerConfig_Sane* m_ScannerConfig;
#endif
    QModelIndex tableView_filelist_pressedQModelIndex;
    QDateTime m_lastclickedtime;
    uint m_lastclickedT;
    QMutex mutex_doScan;
    bool  auto_split;
    bool  auto_scan;
    QString interval_time;
    bool loadFinished = false;
};

class ScannerAPP : public QObject
{
public:
    static void InitAPP();

#ifdef QT_WIN
    static DTwainAPP* m_ScannerCore;
#else
    static Scanner_Operation* m_ScannerCore;
#endif
};

#endif // SCANARCHWINDOW_H
