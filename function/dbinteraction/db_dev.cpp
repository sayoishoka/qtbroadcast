#include "../dbinteraction.h"

#include <QDebug>
#include <QJsonArray>
#include <QJsonObject>
#include <QSqlError>
#include <QSqlQuery>
#include <QSqlDatabase>
#include <QThread>

/** 设备 **/
QVector<Device> SelectDevs()
{
    QSqlQuery query(QSqlDatabase::database(QString::number(reinterpret_cast<quintptr>(QThread::currentThreadId()))));
    QVector<Device> devs;

    if (!query.exec("select * from device;")) {
        qDebug() << "exec SelectDevs() failed!!!";
        qDebug() << query.lastError();
    }

    while (query.next()) {
        Device dev;
        dev.devNo   = query.value("dev_no").toUInt();
        dev.devName = query.value("dev_name").toString().toUtf8().data();
        dev.volume  = query.value("volume").toInt();
        devs.push_back(dev);
    }

    return devs;
}

QVector<Device> SelectGpDevs(uint groupNo, uint devNo, QString devName)
{
    QVector<Device> devs;
    QSqlQuery query(QSqlDatabase::database(QString::number(reinterpret_cast<quintptr>(QThread::currentThreadId()))));
    QString sql("select * from `group_info` natural join `device` "
                "where `gp_no` = " +
                QString::number(groupNo) + " ");
    if (devNo != 0) {
        sql += QString("and `dev_no` = " + QString::number(devNo) + " ");
    }
    if (!devName.isEmpty()) {
        sql += QString("and `dev_name` like '%" + devName + "%' ");
    }

    if (!query.exec(sql)) {
        qDebug() << "exec SelectGpDevs(uint , uint, QString ) failed!!!";
        qDebug() << query.lastError();
    }

    while (query.next()) {
        Device dev;
        dev.devNo   = query.value("dev_no").toUInt();
        dev.devName = query.value("dev_name").toString();
        dev.volume  = query.value("volume").toInt();
        devs.push_back(dev);
    }
    return devs;
}

bool InsertDev(uint devNo, QString devName, uchar volume)
{
    QSqlQuery query(QSqlDatabase::database(QString::number(reinterpret_cast<quintptr>(QThread::currentThreadId()))));
    query.prepare("insert into `device`(dev_no, dev_name, volume) "
                  "values(:devNo, :devName, volume);");
    query.bindValue(":devNo", devNo);
    query.bindValue(":devName", devName);
    query.bindValue(":volume", volume);

    if (!query.exec()) {
        qDebug() << query.lastError();
        return false;
    }
    else {
        return true;
    }
}

bool setDevVolume(uint devNo, uchar volume)
{
    QSqlQuery query(QSqlDatabase::database(QString::number(reinterpret_cast<quintptr>(QThread::currentThreadId()))));
    query.prepare("update `device` set `volume` = :volume where `dev_no` = :devNo;");
    query.bindValue(":devNo", devNo);
    query.bindValue(":volume", volume);

    if (!query.exec()) {
        qDebug() << query.lastError();
        return false;
    }
    else {
        return true;
    }
}

uint getDevNo(QString devName) {
    QSqlQuery query(QSqlDatabase::database(QString::number(reinterpret_cast<quintptr>(QThread::currentThreadId()))));



    query.prepare("SELECT dev_no FROM device WHERE dev_name = :devName");
    query.bindValue(":devName", devName);
    if (query.exec() && query.next()) {
        return query.value(0).toUInt();
    } else {
        // 处理错误
        qDebug() << query.lastError();
        qDebug() << "getDevNo  fail!!";
        return 0;
    }
}

uint getDevGroupNo(int devNo){
    QSqlQuery query(QSqlDatabase::database(QString::number(reinterpret_cast<quintptr>(QThread::currentThreadId()))));
    query.prepare("SELECT gp_no FROM `group_info` WHERE dev_no = :devNo");
    query.bindValue(":devNo", devNo);
    if (query.exec() && query.next()) {
        return query.value(0).toUInt ();
    } else {
        // 处理错误
        qDebug() << query.lastError();
        qDebug() << "getDevGroupNo fail!!";

        return 0;
    }
}
