#ifndef DBUTIL_H
#define DBUTIL_H
#include <QtSql/QSqlTableModel>
#include <QtSql/QSqlQuery>
#include <QStringList>
#include<QSqlRecord>
#include <QtSql/QSqlDatabase>
class DBUtil
{
public:
    explicit DBUtil();
    ~DBUtil();
    static bool opendatabase();
    static void closedatabase();
    static bool insert(QString table, QStringList names,QStringList values);
    static bool Updata(QString table, QStringList names,QStringList values, QString expression);
    static bool del(QString table, QString expression);
    static QList<QStringList>  GetValues(QString table, QString where);
    static QSqlQuery GetQSqlQuery(QString sql);
    static bool ExeSql(QString sql);
private:
    static QSqlDatabase _db;
};

#endif // DBUTIL_H
