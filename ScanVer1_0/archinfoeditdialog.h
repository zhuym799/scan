#ifndef ARCHINFOEDITDIALOG_H
#define ARCHINFOEDITDIALOG_H
#include <QDialog>
namespace Ui
{
class ArchInfoEditDialog;
}

class ArchInfoEditDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ArchInfoEditDialog(QWidget *parent = nullptr);
    ~ArchInfoEditDialog();
    ///设置档案id
    void setID(bool operation,QString pid,QString id);
    /// 获取材料名称
    QString getClmc();
    ///获取材料页数
    QString getYs();
private slots:
    void on_btnCancel_clicked();
    void on_btnOK_clicked();

private:
    Ui::ArchInfoEditDialog *ui;
    ///档案id
    bool operation;
    QString id;
    QString pid;
};

#endif // ARCHINFOEDITDIALOG_H
