#ifndef ORIGPICTUREPROCESSWINDOW_H
#define ORIGPICTUREPROCESSWINDOW_H

#include <QWidget>
#include <QJsonArray>
#include "src/control/imagecontrol/imageview.h"
#include "src/ui/business/imageprocess/dialogbatchimageprocess.h"
#include "src/common/filescontrol.h"

//using namespace cv;
using namespace std;

namespace Ui
{
class OrigPictureProcessWindow;
}
class HDPictureProcessWindow;
class PictureProcessWindow;
class ImageProcessTipWindow;
class BatchChangeAngleWindow;
class QMenu;

class OrigPictureProcessWindow : public QWidget
{
    Q_OBJECT

public:
    explicit OrigPictureProcessWindow(QWidget* parent = nullptr);
    ~OrigPictureProcessWindow();
    void resizeEvent(QResizeEvent* event);
    void InitJPG(QString allfilepath);
    //图像处理页面隐藏 高清页中使用的功能
    void hidePickPhotoButton();

signals:
    void onPicImageViewChangeOperateType(ImageView::ImageOperateType operateType);
private slots:
    ///图像转优化
    void on_btnClearImage_clicked();

    void on_btnAutoCorrectAngle_clicked();
    //手动纠偏
    void on_btnMannualChangeAngle_clicked();
    //切边
    void on_btnCutBorder_clicked();
    //拖拽
    void on_btnDrag_clicked();
    //显示下拉菜单
    void on_btnShowMore_clicked();
    //下拉事件
    void onShowMoreMenutriggered(QAction* action);
    void on_btnUndo_clicked();
    void on_btnRedo_clicked();
    void onMyGetSelectMat(int x, int y, int w, int h, Mat& mat);
    //抠取照片
    void on_btnPickPhoto_clicked();
    //批量纠偏
    void on_btnBatchAutoChangAngle_clicked();
    void on_chkShowRuler_clicked();
    void on_chkScrollBarShow_clicked();
    ///保存文件
    void on_btnSave_clicked();
    ///按键事件
    void onKeyPressEvent(QKeyEvent* event);
    void onGetSelectMat2(int x, int y, int w, int h, Mat& mat);
    ///局部优化
    void onGetPartClearMat(int x, int y, int w, int h, Mat& mat);
    void onGetSelectMat3(int x, int y, int w, int h, Mat& mat);
    ///取红色
    void onGetSelectRed(int x, int y, int w, int h, Mat& mat);
    ///取蓝色
    void onGetSelectBlue(int x, int y, int w, int h, Mat& mat);
    void onGetSelectBlack(int x, int y, int w, int h, Mat& mat);
    ///抠取照片到基本信息
    void on_btnPickPhoto2_clicked();
    void on_chkLocker_clicked();
    void on_btnSet_clicked();
    void on_btnPartClear_clicked();
    void on_edtThreshold_textChanged(const QString& arg1);
    void on_chkAutoThresholdValue_stateChanged(int arg1);
    void on_edtGamma_textChanged(const QString& arg1);
    void on_edtThreshold_MouseDoubleClick();
    void on_btnTip_clicked();

    void focus2yz();
    void focus2Gamma();
    void on_btnAICorrectAngle_clicked();

    void on_btnAIBatCorrectAngle_clicked();

    void on_btnAICutBorder1_clicked();

    void on_btnAIBatCutBorder1_clicked();

    void on_btnAIRemoveStain_clicked();

    void on_btnAIBatRemoveStain_clicked();

    void on_btnAICutBorder2_clicked();

    void on_btnAIBatCutBorder2_clicked();

public:
    //优化窗口
    HDPictureProcessWindow* HdWindow;
    ImageProcessTipWindow* imageProcessTipWindow;
    PictureProcessWindow* PicWin;
    QMenu* showMoreMenu;
    BatchChangeAngleWindow* batChangeWindow;
    QString m_A0100;
private:
    Ui::OrigPictureProcessWindow* ui;
};

#endif // ORIGPICTUREPROCESSWINDOW_H
