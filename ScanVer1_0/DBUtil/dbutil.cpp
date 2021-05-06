#include "dbutil.h"
#include <QtSql/QSqlQuery>
#include <qdebug.h>
#include <QApplication>
QSqlDatabase DBUtil::_db;
DBUtil::DBUtil()
{

}

DBUtil::~DBUtil()
{
    _db.close();
}


bool DBUtil::opendatabase()
{
    if (QSqlDatabase::contains("qt_sql_default_connection"))
    {
        _db = QSqlDatabase::database("qt_sql_default_connection");
    }
    else
    {
        _db = QSqlDatabase::addDatabase("QSQLITE");
    }
    QString apppath = QApplication::applicationDirPath() + "/db/ws.db";
    _db.setDatabaseName(apppath);
    if (!_db.open())
    {
        return  false;
    }
    else
    {
        return  true;
    }

}

void DBUtil::closedatabase()
{

    QString name;
    {
        name = _db.connectionName();
    }
    _db.removeDatabase(name);
}



bool DBUtil::insert(QString &table, QStringList &names, QStringList &values)
{
    if (names.size() != values.size())
    {
        return false;
    }

    QSqlQuery query(_db);
    if (opendatabase())
    {
        QString sql = QString("insert into ") + table + QString("(");

        int i;

        for (i=0; i < names.size(); i++)
        {
            sql = sql + names.value(i);
            if (i != names.size() - 1)
            {
                sql+=QString(",");
            }
            else
            {

                sql = sql + QString(")");
            }
        }

        sql = sql + QString("values (");

        for (i = 0; i < values.size(); i++)
        {
            sql = sql + QString("'") + values.value(i) + QString("'");
            if (i != values.size()-1)
            {
                sql = sql + QString(",");
            }
        }

        sql = sql + QString(")");
        //qDebug()<<sql;
        if (query.exec(sql))
        {
            return true;
        }
        else
        {

            return false;
        }
    }
}

bool DBUtil::ExeSql(QString &sql){

    QSqlQuery query(QSqlDatabase::database());
    if (query.exec(sql))
    {
        return true;
    }
    else
    {
        return false;
    }

}

bool DBUtil::Updata(QString &table, QStringList &names, QStringList &values, QString &expression)
{
    if (names.size() != values.size())
    {
        return false;
    }


    QSqlQuery query(_db);
    if (opendatabase()){
        QString sql = QString("update ")+table+QString(" set ");
        for (int i = 0; i < names.size(); i++)
        {
            sql = sql + names.value(i);
            sql = sql + QString(" = '");
            sql = sql + values.value(i);
            sql = sql + QString("'");
            if (i != names.size()-1)
            {
                sql = sql + QString(" ,");
            }
        }

        sql = sql + QString(" where ") + expression;
        if (query.exec(sql))
        {
            return true;
        }
        else
        {
            return false;
        }
    }
}


bool DBUtil::del(QString &table, QString &expression)
{

    QSqlQuery query(_db);

    QString sql = QString("delete from ") + table + QString(" where ") + expression;
    if (opendatabase()){
        if (query.exec(sql))
        {
            return true;
        }
        else
        {
            return false;
        }
    }
}
QList<QStringList>  DBUtil::GetValues(QString table, QString where)
{
    QList<QStringList> list;
    QSqlQuery query(_db);
    if (opendatabase()){
        QString sql = QString("select * from ") + table +where;
        query.exec(sql);
        QSqlRecord rec = query.record();
        int row=rec.count();
        while (query.next())
        {
            QStringList values;
            for (int i = 0; i < row; ++i) {
                values << query.value(i).toString();
            }
            list.append(values);

        }
    }
    return  list;
}

QSqlQuery DBUtil::GetQSqlQuery(QString &sql)
{
    QSqlQuery query(QSqlDatabase::database());
    query.setForwardOnly(true);
    query.prepare(sql);
    query.exec();

    return query;
}
