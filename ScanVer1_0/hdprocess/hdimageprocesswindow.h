#ifndef HDIMAGEPROCESSWINDOW_H
#define HDIMAGEPROCESSWINDOW_H

#include <QMainWindow>
#include "src/control/imagecontrol/imageview.h"

namespace Ui
{
class HDImageProcessWindow;
}

class QTableView;
class QStandardItem;
class QStandardItemModel;
class BatchangeHdWindow;
class SearchPersonInfoExForm;
class ArchInfoHelper;
class OrigPictureProcessWindow;
class HDPictureProcessWindow;
class DownLoadAndUpLoadProgress;
class QSplitter;
class TSelectedCatalogInfo;
class DownLoadAndUpLoadThreadTaskInfo;

class HDImageProcessWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit HDImageProcessWindow(QWidget* parent = nullptr);
    ~HDImageProcessWindow();

    //Treeview跳到下一个节点
    QStandardItem* getNext_LX1_TreeViewNode(QModelIndex qBeginModelIndex);
    //Treevie 跳到上一个节点
    QStandardItem* getPre_LX1_TreeViewNode(QModelIndex qBeginModelIndex);
    void DownLoadFile();

    //加载数据
    void resizeEvent(QResizeEvent* event);
    void setLoadedFinish(bool bFinished);
protected:
    virtual void closeEvent(QCloseEvent* event);

private:
    void keyPressEvent(QKeyEvent* event);
    bool eventFilter(QObject* obj, QEvent* e);

    //   显示上传下载进度窗口
    void ShowDownLoadAndUpLoadProgress(int type, QList<DownLoadAndUpLoadThreadTaskInfo*> tasklist);
    //   上传原始图像
    void UpLoadYSFile();
    //   上传优化图像
    void UpLoadGQFile();
    //   获取相对路径
    void getRelativeFiles(QString pathType, QStringList& files);
    //   获取路径
    void getFullPathFiles(QString pathType, QStringList& files);

signals:
    void refreshArchInfos();

private slots:
    void sl_DownLoadAndUpLoadProgressOnClose();
    ///点击类别节点事件
    void on_u_treeView_catalog_clicked(const QModelIndex& index);

    void on_pushButton_clicked();

    void on_pushButton_3_clicked();

    void on_pushButton_4_clicked();

    void on_u_tableView_filelist_clicked(const QModelIndex& index);
    //批量转优化
    void on_btnBatchClearImage_clicked();
    //下一页
    void on_btnNextPage_clicked();
    //上一页
    void on_btnPrePage_clicked();
    ///右键菜单事件
    void onControlTriggered();

    void on_btnClear_clicked();

    void on_u_treeView_catalog_customContextMenuRequested(const QPoint& pos);

    void on_u_tableView_filelist_customContextMenuRequested(const QPoint& pos);

    void onResize();
    void onSelectPerson(QTableView* tableView, QString id, QString xm, QString cardNum);
    void onLoadData(QString a0100);
    void onSrcImageViewChangeOperateType(ImageView::ImageOperateType operateType);
    void onHdImageViewChangeOperateType(ImageView::ImageOperateType operateType);
    void showMessage(int type, QString title, QString content);

public:
    QList<QStandardItem*> items;
    //转优化窗口
    BatchangeHdWindow* batChangeHdWindow;
    SearchPersonInfoExForm* searchPersonInfoExForm;
    ArchInfoHelper* archInfoHelper;
    //原始图像处理窗口
    OrigPictureProcessWindow* u_frame_orig;
    //优化图像处理窗口
    HDPictureProcessWindow* u_frame_hd;

private:
    Ui::HDImageProcessWindow* ui;
    DownLoadAndUpLoadProgress* winDownUpProgress = nullptr;
    QStandardItemModel* qsimCatalog;
    QStandardItemModel* qsimFilesList;
    ///分割原始和优化窗口
    QSplitter* spl1;
    QSplitter* spl2;
    //目录选中时候ID
    TSelectedCatalogInfo* selectedCatalogInfo;
    bool loadFinished = false;
};

#endif // HDIMAGEPROCESSWINDOW_H
