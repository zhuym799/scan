#ifndef FILEDIRECTORYFORM_H
#define FILEDIRECTORYFORM_H

#include <QWidget>

namespace Ui {
class FileDirectoryForm;
}

class FileDirectoryForm : public QWidget
{
    Q_OBJECT

public:
    explicit FileDirectoryForm(QWidget *parent = nullptr);
    ~FileDirectoryForm();

private:
    Ui::FileDirectoryForm *ui;
};

#endif // FILEDIRECTORYFORM_H
