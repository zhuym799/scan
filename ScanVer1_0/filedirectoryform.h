#ifndef FILEDIRECTORYFORM_H
#define FILEDIRECTORYFORM_H

#include <QWidget>
#include <QStandardItemModel>
#include <QDebug>
#include <QAction>
#include "archinfoeditdialog.h"
#include "qscanglobal.h"
struct TreeViewDepart
{
    QString pid;//上级单位序号
    QString id;//B00id
    QString name;//机构编码
    QStringList Values;
};
Q_DECLARE_METATYPE(TreeViewDepart)
namespace Ui {
class FileDirectoryForm;
}

class FileDirectoryForm : public QWidget
{
    Q_OBJECT
public:
    explicit FileDirectoryForm(QWidget *parent = nullptr);
    ~FileDirectoryForm();
    void init();
    void loadusergroup();
     void loadItem(QStandardItemModel* model, QStandardItem* parentItem, QString parentId, QList<TreeViewDepart>& list);

private slots:
    void OnContextMenuPop();
private:
    Ui::FileDirectoryForm *ui;
    QStandardItemModel* model;
    QString pid;
    QString uid;
};

#endif // FILEDIRECTORYFORM_H
