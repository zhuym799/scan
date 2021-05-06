#include "qscanglobal.h"
QScanGlobal QScanGlobal::instance;
QScanGlobal::QScanGlobal()
{

}
QList<QStringList>  QScanGlobal::selectAllUser(QString where){

    return DBUtil::GetValues(t_user,where);
}
