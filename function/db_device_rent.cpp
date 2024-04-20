#include "db_device_rent.h"

db_device_rent::db_device_rent(QObject *parent) : QObject(parent)
{
    Dispatcher::getDispatcher()->Register("getAddress",std::bind(&db_device_rent::getAddress, this,std::placeholders::_1));
    Dispatcher::getDispatcher()->Register("getRentStatus",std::bind(&db_device_rent::getRentStatus, this,std::placeholders::_1));
    Dispatcher::getDispatcher()->Register("getSomeStatus",std::bind(&db_device_rent::getSomeStatus, this,std::placeholders::_1));
}

QJsonObject db_device_rent::getAddress(QJsonObject &obj)
{
    QJsonObject requestjson;
    QJsonArray List;

    dbSelectUpdate e;
    QSqlQuery query = e.getData_Sheet("SELECT * FROM device");
    while(query.next()){
        QJsonObject dev;
        dev.insert("devNo",query.value(0).toInt());
        dev.insert("longtitude",query.value(3).toDouble());
        dev.insert("latitude",query.value(4).toDouble());
        List.append(dev);
    }
    requestjson.insert("response","reGetAddress");
    requestjson.insert("List",List);
    requestjson.insert("status",true);
    return requestjson;
}

QJsonObject db_device_rent::getRentStatus(QJsonObject &obj)
{
    QJsonObject requestjson;
    QJsonArray rentList;

    dbSelectUpdate e;
    QSqlQuery query = e.getData_Sheet("SELECT * FROM dev_rent");
    while(query.next()){
        QJsonObject dev;
        dev.insert("devNo",query.value(1).toInt());
        dev.insert("devName",query.value(2).toString());
        dev.insert("currentStatus",query.value(3).toInt());
        dev.insert("remaining",query.value(4).toInt());
        dev.insert("usedTime",query.value(5).toInt());
        rentList.append(dev);
    }
    requestjson.insert("response","reGetRentStatus");
    requestjson.insert("rentList",rentList);
    requestjson.insert("status",true);
    return requestjson;
}

QJsonObject db_device_rent::getSomeStatus(QJsonObject &obj)
{
    QString devNo = QString::number(obj.value("devNo").toInt());
    QString devName = obj.value("devName").toString();
    QString currentStatus = QString::number(obj.value("currentStatus").toInt());

    QJsonObject requestjson;
    QJsonArray rentList;
    dbSelectUpdate e;
    QSqlQuery query = e.getData_Sheet("SELECT * FROM dev_rent "
                                      "WHERE dev_no LIKE '%"+devNo+"%' "
                                      "AND dev_name LIKE '%"+devName+"%' "
                                      "AND rental_status LIKE '%"+currentStatus+"%'");
    while(query.next()){
        QJsonObject dev;
        dev.insert("devNo",query.value(1).toInt());
        dev.insert("devName",query.value(2).toString());
        dev.insert("currentStatus",query.value(3).toInt());
        dev.insert("remaining",query.value(4).toInt());
        rentList.append(dev);
    }
    requestjson.insert("response","reGetSomeStatus");
    requestjson.insert("rentList",rentList);
    requestjson.insert("status",true);
    return requestjson;
}
