#include "qscanglobal.h"
#include <QUuid>
#include <QMessageBox>
QScanGlobal QScanGlobal::instance;
QScanGlobal::QScanGlobal()
{

}
QList<QStringList>  QScanGlobal::selectAllUser(QString where){
    return DBUtil::GetValues(t_user,where);
}
bool  QScanGlobal::addFileDirectory(QStringList names, QStringList values){
    return DBUtil::insert(t_file,names,values);
};
bool QScanGlobal::delFileDirectory(QString expressio){
    return DBUtil::del(t_file,expressio);
}
bool QScanGlobal::updateFileDirectory(QStringList names, QStringList values, QString expression){
  return DBUtil::Updata(t_file,names,values,expression);
}
QList<QStringList>  QScanGlobal::selectAllFileDirectory(QString where){
    return DBUtil::GetValues(t_file,where);
}
int QScanGlobal::question(QWidget* parent, const QString& title, const QString& text)
{
    Q_UNUSED(parent);
    QMessageBox box(QMessageBox::Question, title, text, QMessageBox::Yes | QMessageBox::No);
    box.setButtonText(QMessageBox::Yes, "确定");
    box.setButtonText(QMessageBox::No, "取消");
    box.setDefaultButton(QMessageBox::No);
    return box.exec();
}

void QScanGlobal::information(QWidget* parent, const QString& title,
                              const QString& text)
{
    Q_UNUSED(parent);
    QMessageBox box(QMessageBox::Information, title, text);
    box.setButtonText(QMessageBox::Ok, "确定");
    box.exec();
}
QString QScanGlobal::CreateGuid()
{
    QUuid uuid=QUuid::createUuid();
    QString s=uuid.toString();
    s=s.remove("{");
    s=s.remove("}");
    return s;
}
