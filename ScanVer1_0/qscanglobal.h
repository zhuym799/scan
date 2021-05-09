#ifndef QSCANGLOBAL_H
#define QSCANGLOBAL_H
#include "DBUtil/dbutil.h"
const QString t_user="UserInfo";
const QString t_file="FileTable";
const QString softwares="文书扫描管理系统";
const QString p_id = "00000000-0000-0000-0000-000000000000";
class QScanGlobal
{
public:
    QScanGlobal();
public:
    static QScanGlobal instance;
public:
    QList<QStringList>  selectAllUser(QString where="");
    bool addFileDirectory(QStringList names, QStringList values);
    QList<QStringList>  selectAllFileDirectory(QString where="");
    bool delFileDirectory(QString expressio);
    bool updateFileDirectory(QStringList names, QStringList values, QString expression);
    QString CreateGuid();
    int  question(QWidget* parent, const QString& title, const QString& text);
    void information(QWidget* parent, const QString& title,
                     const QString& text);
};

#endif // QSCANGLOBAL_H
