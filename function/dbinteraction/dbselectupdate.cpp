#include "dbselectupdate.h"
dbSelectUpdate::dbSelectUpdate(QObject *parent)
    : QObject{parent}
{

}


QSqlQuery dbSelectUpdate::getData_Sheet(QString sheet)
{
    QSqlQuery query;
    query.exec(sheet);
    return query;
}


void dbSelectUpdate::exeSql(QString sheet)
{
    QSqlQuery query;
    query.exec(sheet);
}
