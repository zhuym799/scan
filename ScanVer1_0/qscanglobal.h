#ifndef QSCANGLOBAL_H
#define QSCANGLOBAL_H
#include "DBUtil/dbutil.h"
const QString t_user="UserInfo";
const QString d_name="MyDataBase.db";
const QString softwares="文书扫描管理系统";
class QScanGlobal
{
public:
    QScanGlobal();
public:
    static QScanGlobal instance;
public:
    QList<QStringList>  selectAllUser(QString where="");
};

#endif // QSCANGLOBAL_H
