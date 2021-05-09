#ifndef HDPICTUREPROCESSWINDOW_H
#define HDPICTUREPROCESSWINDOW_H

#include "src/ui/business/imageprocess/pictureprocesswindow.h"
#include <QVBoxLayout>
#include <QWidget>

#include "origpictureprocesswindow.h"
#include "src/ui/business/imageprocess/changegammawindow.h"
#include <QIntValidator>
#include <QMenu>

namespace Ui
{
class HDPictureProcessWindow;
}
class OrigPictureProcessWindow;
class HDPictureProcessWindow : public QWidget
{
    Q_OBJECT

public:
    explicit HDPictureProcessWindow(QWidget* parent = nullptr);
    ~HDPictureProcessWindow();

private:
    Ui::HDPictureProcessWindow* ui;

public:
    void InitJPG(QString allfilepath);
    PictureProcessWindow* PicWin;
    QMenu* showMoreMenu;
    OrigPictureProcessWindow* OrgWindow;
    ///改变GammaWindow
    ChangeGammaWindow* changeGammaWindow;
    ///采用原始图像
    void useOrigialImage();
private slots:
    //橡皮擦
    void on_btnErase_clicked();
    void on_edtEraserWidth_textChanged(const QString& arg1);
    void on_btnShowMore_clicked();
    //下拉事件
    void onShowMoreMenutriggered(QAction* action);
    void on_btnClearSel_clicked();
    void on_btnClearDeSelect_clicked();
    void on_btnFillHole_clicked();
    void on_btnChangeMixColor_clicked();
    void on_btnChangeMixColor2_clicked();
    void on_btnUndo_clicked();
    void on_btnRedo_clicked();
    void on_btnSaveFile_clicked();
    void on_btnRemoveStains_clicked();
    void on_toolButton_clicked();
    void on_btnDrag_clicked();
    void on_chkMarkRuler_clicked();
    void on_chkScrollBar_clicked();
    void on_edtStainWidth_textChanged(const QString& arg1);
    ///按键事件
    void onKeyPressEvent(QKeyEvent* event);
    ///Gamma值设置
    void on_btnGamma_clicked();
    ///Gamma调整
    void onGetSelectGamma(int x, int y, int w, int h, Mat& mat);
    void onChangeGamma(int x, int y, int w, int h, Mat& mat);
    void on_chkCenter_stateChanged(int arg1);
    void on_btnGetColor_clicked();
    void on_btnStamp_clicked();
    void on_edtStampWith_textChanged(const QString& arg1);
    void focus2xpc();
    void focus2zs();
    void focus2wd();
};

#endif // HDPICTUREPROCESSWINDOW_H
