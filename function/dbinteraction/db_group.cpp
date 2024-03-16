#include "../dbinteraction.h"

#include <QDebug>
#include <QJsonArray>
#include <QJsonObject>
#include <QSqlError>
#include <QSqlQuery>

/** 分组 **/
QVector<Group> GetAllGroup()
{
    QVector<Group> groups;
    QSqlQuery query(QSqlDatabase::database(QString::number(reinterpret_cast<quintptr>(QThread::currentThreadId()))));

    if (!query.exec("select * from `group`;")) {
        qDebug() << "exec ReAddGroup failed!!!";
        qDebug() << query.lastError();
    }

    Group gp;
    while (query.next()) {
        gp.groupNo   = query.value("gp_no").toUInt();
        gp.groupName = query.value("gp_name").toString();
        groups.push_back(gp);
    }

    return groups;
}

bool InsertGroup(QString groupName, uint groupNo)
{
    QSqlQuery query(QSqlDatabase::database(QString::number(reinterpret_cast<quintptr>(QThread::currentThreadId()))));

    query.prepare("insert into `group`(gp_name, gp_no) "
                  "values(:groupName, :groupNo);");
    query.bindValue(":groupName", groupName);
    query.bindValue(":groupNo", groupNo);

    if (!query.exec()) {
        qDebug() << "exec InsertGroup failed!!!";
        qDebug() << query.lastError();
        return false;
    }
    else {
        return true;
    }
}

bool DeleteGroup(uint groupNo)
{
    QSqlQuery query(QSqlDatabase::database(QString::number(reinterpret_cast<quintptr>(QThread::currentThreadId()))));
    query.prepare("delete from `group` where gp_no = :groupNo;");
    query.bindValue(":groupNo", groupNo);

    if (!query.exec()) {
        qDebug() << "exec DeleteGroup failed!!!";
        qDebug() << query.lastError();
        return false;
    }
    else {
        return true;
    }
}

bool UpDateGroupName(uint groupNo, QString groupName)
{
    QSqlQuery query(QSqlDatabase::database(QString::number(reinterpret_cast<quintptr>(QThread::currentThreadId()))));
    query.prepare("update `group` set `gp_name` = :groupName "
                  "where `gp_no` = :groupNo;");
    query.bindValue(":groupName", groupName);
    query.bindValue(":groupNo", groupNo);

    if (!query.exec()) {
        qDebug() << "exec UpDataGroupName failed!!!";
        qDebug() << query.lastError();
        return false;
    }
    else {
        return true;
    }
}

bool InsertDevToGroup(uint devNo, uint groupNo)
{
    QSqlQuery query(QSqlDatabase::database(QString::number(reinterpret_cast<quintptr>(QThread::currentThreadId()))));
    query.prepare("insert into `group_info`(`dev_no`, `gp_no`) "
                  "values(:devNo, :groupNo)");
    query.bindValue(":devNo", devNo);
    query.bindValue(":groupNo", groupNo);

    if (!query.exec()) {
        qDebug() << "exec InsertDevToGroup failed!!!";
        qDebug() << query.lastError();
        return false;
    }
    else {
        return true;
    }
}

bool DeleteDevToGroup(uint devNo)
{
    QSqlQuery query(QSqlDatabase::database(QString::number(reinterpret_cast<quintptr>(QThread::currentThreadId()))));
    query.prepare("delete from `group_info` "
                  "where `dev_no` = :devNo;");
    query.bindValue(":devNo", devNo);

    if (!query.exec()) {
        qDebug() << "exec DeleteDevToGroup failed!!!";
        qDebug() << query.lastError();
        return false;
    }
    else {
        return true;
    }
}

uint getGroupNo(QString groupName) {
    QSqlQuery query(QSqlDatabase::database(QString::number(reinterpret_cast<quintptr>(QThread::currentThreadId()))));
    query.prepare("SELECT gp_no FROM `group` WHERE gp_name = :groupName ");

    query.bindValue(":groupName", groupName);
    if (query.exec() && query.next()) {
        return query.value(0).toUInt();
    } else {
        // 处理错误
        return 0;
    }
}
