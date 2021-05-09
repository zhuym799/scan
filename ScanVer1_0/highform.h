#ifndef HIGHFORM_H
#define HIGHFORM_H

#include <QWidget>

namespace Ui {
class HighForm;
}

class HighForm : public QWidget
{
    Q_OBJECT

public:
    explicit HighForm(QWidget *parent = nullptr);
    ~HighForm();

private:
    Ui::HighForm *ui;
};

#endif // HIGHFORM_H
