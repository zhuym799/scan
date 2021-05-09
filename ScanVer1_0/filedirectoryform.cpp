#include "filedirectoryform.h"
#include "ui_filedirectoryform.h"
FileDirectoryForm::FileDirectoryForm(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::FileDirectoryForm)
{
    ui->setupUi(this);
    init();
    loadusergroup();

}
void FileDirectoryForm::init(){
    model=new QStandardItemModel();
    ui->treeView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->treeView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->treeView->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->treeView->setModel(model);
    ui->treeView->setMouseTracking(true);
    ui->treeView->setContextMenuPolicy(Qt::ActionsContextMenu);
    ui->treeView->setHeaderHidden(true);
    //添加右键菜单

    QAction* act = new QAction(ui->treeView);
    act->setIcon(QIcon(":image/add.ico"));
    connect(act, SIGNAL(triggered()), this, SLOT(OnContextMenuPop()));
    act->setText("添加");
    ui->treeView->addAction(act);
    act = new QAction(ui->treeView);
    connect(act, SIGNAL(triggered()), this, SLOT(OnContextMenuPop()));
    act->setIcon(QIcon(":image/update.ico"));
    act->setText("修改");
    ui->treeView->addAction(act);
    act = new QAction(ui->treeView);
    act->setIcon(QIcon(":image/del.ico"));
    connect(act, SIGNAL(triggered()), this, SLOT(OnContextMenuPop()));
    act->setText("删除");

    ui->treeView->addAction(act);
}
void FileDirectoryForm::OnContextMenuPop(){
    QAction* act = static_cast<QAction*>(sender());
    QString text = act->text();
    QModelIndex index = ui->treeView->currentIndex();
    QStandardItem* item =model ->itemFromIndex(index);
    QString  id;
    QString  pid;
    if(text=="添加"){
        if (item == nullptr){
            pid=p_id;
        }else{
            pid= item->data(Qt::UserRole + 2).toString();
        }
        id=QScanGlobal::instance.CreateGuid();
        ArchInfoEditDialog* dlg = new ArchInfoEditDialog(this);
        dlg->setID(false,pid,id);
        if (dlg->exec() == QDialog::Accepted)
        {
            loadusergroup();
        }
    }else if(text=="修改"){
        if (item == nullptr){
            return;
        }else{
            id= item->data(Qt::UserRole + 2).toString();
        }
        ArchInfoEditDialog* dlg = new ArchInfoEditDialog(this);
        dlg->setID(true,pid,id);
        if (dlg->exec() == QDialog::Accepted)
        {
            loadusergroup();
        }
    }else if(text=="删除"){
        if (item == nullptr){
            return;
        }
        id = item->data(Qt::UserRole + 2).toString();
        QString where=" id ='"+id+"' or pid='"+id+"'";
        if(QScanGlobal::instance.delFileDirectory(where)){
            QScanGlobal::instance.information(this,"提示","删除成功！");
            loadusergroup();
        }else{
            QScanGlobal::instance.information(this,"提示","删除失败！");
        }
    }

}
FileDirectoryForm::~FileDirectoryForm()
{
    delete ui;
}


void FileDirectoryForm::loadusergroup()
{
    model->clear();
    QList<QStringList> list=QScanGlobal::instance.selectAllFileDirectory();
    QList<TreeViewDepart> dataList;
    for (int i = 0; i < list.count(); i++)
    {

        QStringList values=list[i];
        TreeViewDepart data;
        data.pid = values.at(0);
        data.id = values.at(1);
        data.name = values.at(2);
        dataList << data;
    }
    loadItem(model, nullptr, p_id, dataList);
    ui->treeView->expandAll();
}
void FileDirectoryForm::loadItem(QStandardItemModel* model, QStandardItem* parentItem, QString parentId, QList<TreeViewDepart>& list)
{
    for (int i = 0; i < list.count(); i++)
    {
        TreeViewDepart data = list.at(i);
        if (data.pid == parentId)
        {
            QStandardItem* item = new QStandardItem();
            item->setSizeHint(QSize(100, 28));
            item->setText(data.name);
            item->setData(data.pid, Qt::UserRole + 1);
            item->setData(data.id, Qt::UserRole + 2);
            if (parentItem == nullptr)
                model->appendRow(item);
            else
                parentItem->appendRow(item);
            loadItem(model, item, data.id, list);
        }
    }
}
