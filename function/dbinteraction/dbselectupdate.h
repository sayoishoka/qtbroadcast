#ifndef DBSELECTUPDATE_H
#define DBSELECTUPDATE_H

#include "qsqlquery.h"
#include <QObject>

class dbSelectUpdate : public QObject
{
    Q_OBJECT
public:
    explicit dbSelectUpdate(QObject *parent = nullptr);

    QSqlQuery getData_Sheet(QString sheet);
    void exeSql(QString sheet);
signals:
};

#endif // DBSELECTUPDATE_H
