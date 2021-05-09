#ifndef PDFFORM_H
#define PDFFORM_H

#include <QWidget>

namespace Ui {
class PdfForm;
}

class PdfForm : public QWidget
{
    Q_OBJECT

public:
    explicit PdfForm(QWidget *parent = nullptr);
    ~PdfForm();

private:
    Ui::PdfForm *ui;
};

#endif // PDFFORM_H
