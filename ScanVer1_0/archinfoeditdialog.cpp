#include "archinfoeditdialog.h"
#include "ui_archinfoeditdialog.h"
#include <QJsonObject>
#include <QRegExpValidator>
#include "qscanglobal.h"

ArchInfoEditDialog::ArchInfoEditDialog(QWidget* parent)
    : QDialog(parent)
    , ui(new Ui::ArchInfoEditDialog)
{
    ui->setupUi(this);
    ui->edtYear->setValidator(new QRegExpValidator(QRegExp("^([1-2]\\d{3})$")));
    ui->edtMonth->setValidator(new QRegExpValidator(QRegExp("^([1-9]|1[0-2])$")));
    ui->edtDay->setValidator(new QRegExpValidator(QRegExp("^([1-9]|((1|2)[0-9])|30|31)$")));
    Qt::WindowFlags flags = this->windowFlags();
    setWindowFlags(flags & ~Qt::WindowContextHelpButtonHint);
}

ArchInfoEditDialog::~ArchInfoEditDialog()
{
    delete ui;
}

void ArchInfoEditDialog::setID(bool operation,QString pid,QString id)
{
    this->id = id;
    this->pid = pid;
    this->operation=operation;
    if(operation){
        QString where=" where id='"+id+"'";
        QList<QStringList> list=QScanGlobal::instance.selectAllFileDirectory(where);
        if(list.count()>0){
            ui->edtClmc->setText(list[0].at(2));
            ui->edtYear->setText(list[0].at(3));
            ui->edtMonth->setText(list[0].at(4));
            ui->edtDay->setText(list[0].at(5));
            ui->edtYs->setText(list[0].at(6));
            ui->edtremarks->setText(list[0].at(7));
        }
    }

}

QString ArchInfoEditDialog::getClmc()
{
    return ui->edtClmc->text();
}

QString ArchInfoEditDialog::getYs()
{
    return ui->edtYs->text();
}

void ArchInfoEditDialog::on_btnCancel_clicked()
{
    reject();
}
void ArchInfoEditDialog::on_btnOK_clicked()
{
    if(operation){
        QStringList names={"name","year","month","day","numberpages","remarks"};
        QStringList values={ui->edtClmc->text(),ui->edtYear->text(),ui->edtMonth->text(),ui->edtDay->text(),ui->edtYs->text(),ui->edtremarks->text()};
        QString where=" id='"+id+"'";
        if(QScanGlobal::instance.updateFileDirectory(names,values,where))
            accept();
        else
            QScanGlobal::instance.information(this,"提示","保存失败");
    }else{
        QStringList names={"pid","id","name","year","month","day","numberpages","remarks"};
        QStringList values={pid,id,ui->edtClmc->text(),ui->edtYear->text(),ui->edtMonth->text(),ui->edtDay->text(),ui->edtYs->text(),ui->edtremarks->text()};
        if(QScanGlobal::instance.addFileDirectory(names,values))
            accept();
        else
            QScanGlobal::instance.information(this,"提示","保存失败");
    }

}
