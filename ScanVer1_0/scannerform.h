#ifndef SCANNERFORM_H
#define SCANNERFORM_H

#include <QWidget>

namespace Ui {
class ScannerForm;
}

class ScannerForm : public QWidget
{
    Q_OBJECT

public:
    explicit ScannerForm(QWidget *parent = nullptr);
    ~ScannerForm();

private:
    Ui::ScannerForm *ui;
};

#endif // SCANNERFORM_H
