#ifndef HANDLEFORM_H
#define HANDLEFORM_H

#include <QWidget>

namespace Ui {
class HandleForm;
}

class HandleForm : public QWidget
{
    Q_OBJECT

public:
    explicit HandleForm(QWidget *parent = nullptr);
    ~HandleForm();

private:
    Ui::HandleForm *ui;
};

#endif // HANDLEFORM_H
