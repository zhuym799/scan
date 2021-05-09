#ifndef IMAGEPROCESSTIPWINDOW_H
#define IMAGEPROCESSTIPWINDOW_H

#include <QMainWindow>

namespace Ui {
class ImageProcessTipWindow;
}

class ImageProcessTipWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit ImageProcessTipWindow(QWidget *parent = nullptr);
    ~ImageProcessTipWindow();

private:
    Ui::ImageProcessTipWindow *ui;
};

#endif // IMAGEPROCESSTIPWINDOW_H
