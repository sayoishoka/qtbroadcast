#ifndef DB_DEVICE_RENT_H
#define DB_DEVICE_RENT_H

#include <QObject>
#include <QSqlQuery>
#include <QJsonObject>
#include <QJsonArray>
#include "connect/dispatcher.h"
#include "function/dbinteraction/dbselectupdate.h"
class db_device_rent : public QObject
{
    Q_OBJECT
public:
    static db_device_rent* getdb_device_rent(){
        static db_device_rent instance;
        return &instance;
    };


signals:


private:
    QJsonObject getAddress(QJsonObject &obj);
    QJsonObject getRentStatus(QJsonObject &obj);
    QJsonObject getSomeStatus(QJsonObject &obj);
    explicit db_device_rent(QObject *parent = nullptr);
};

#endif // DB_DEVICE_RENT_H
