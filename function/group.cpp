#include "function/group.h"
#include "dbinteraction.h"
#include "connect/dispatcher.h"
#include "qjsonobject.h"
#include<daotype.h>
#include <lib/NAudioServerLib.h>
#include <QDebug>
#include <QJsonArray>
#include <QJsonObject>
#include <QSqlError>
#include <QSqlQuery>
#include <QVector>

GroupModule *GroupModule::getInstance()
{
    static GroupModule groupModule;
    return &groupModule;
}

GroupModule::GroupModule(QObject *parent)
    : QObject(parent)
{
    InitMapper();
    qDebug() << "GroupModule load!";
}


/* 功能部分 */
void GroupModule::InitMapper()
{
    using namespace std::placeholders;
    // 类型别名
    using Func = std::function<QJsonObject(QJsonObject &)>;
    // 引用分发器的指针
    Dispatcher *dispatcher = Dispatcher::getDispatcher();

    /* 注册功能 */
    /* 客户端部分 */
    Func reGroupList = std::bind(&GroupModule::ReGroupList, this, _1);
    dispatcher->Register("getGroupList", reGroupList);

    Func reAddGroup = std::bind(&GroupModule::ReAddGroup, this, _1);
    dispatcher->Register("addGroup", reAddGroup);

    Func reDeleteGroup = std::bind(&GroupModule::ReDeleteGroup, this, _1);
    dispatcher->Register("deleteGroup", reDeleteGroup);

    Func reUpDateGroupName = std::bind(&GroupModule::ReUpDateGroupName, this, _1);
    dispatcher->Register("updateGroupName", reUpDateGroupName);

    Func reAddDevToGroup = std::bind(&GroupModule::ReAddDevToGroup, this, _1);
    dispatcher->Register("addDevToGroup", reAddDevToGroup);

    Func reDeleteDevToGroup = std::bind(&GroupModule::ReDeleteDevToGroup, this, _1);
    dispatcher->Register("deleteDevToGroup", reDeleteDevToGroup);

    Func reModfiyDevToGroup = std::bind(&GroupModule::ReModifyDevToGroup, this, _1);
    dispatcher->Register("modifyDevToGroup", reModfiyDevToGroup);
    
    Func reInquireDevToGroup = std::bind(&GroupModule::ReInquireDevToGroup, this, _1);
    dispatcher->Register("inquireDevToGroup", reInquireDevToGroup);
}


/* 接口处理 */
QJsonObject GroupModule::ReGroupList(QJsonObject &data)
{
    QJsonObject obj;
    obj.insert("response", "reGetGroupList");
    qDebug()<<"reGetGroupList";

    QJsonArray gpsJson;
    QVector<Group> gps = GetAllGroup();
    for (auto gp : gps) {
        QJsonObject gpJson;
        gpJson.insert("groupName", gp.groupName);
        gpJson.insert("groupNo", (int)gp.groupNo);

        gpsJson.append(gpJson);
    }
    obj.insert("grouplist", gpsJson);

    return obj;
}

QJsonObject GroupModule::ReAddGroup(QJsonObject &data)
{
    qDebug()<<"!@#";
    uint groupNo      = (uint)data.value("groupNo").toInt();
    QString groupName = data.value("groupName").toString();
    qDebug()<<QString::number(groupNo)<<groupName;
    bool status = InsertGroup(groupName, groupNo);
    // 调用服务端接口
    if (status) {
        char *gp_name = groupName.toUtf8().data();
        na_add_castgroup(groupNo, gp_name);
    }
    QJsonObject obj;
    obj.insert("response", "reAddGroup");
    obj.insert("status", status);

    return obj;
}

QJsonObject GroupModule::ReDeleteGroup(QJsonObject &data)
{
    uint groupNo = (uint)data.value("groupNo").toInt();
    bool status  = DeleteGroup(groupNo);
    // 调用服务端接口
    if (status) {
        na_remove_castgroup(groupNo);
    }
    QJsonObject obj;
    obj.insert("response", "reUpdataGroupName");
    obj.insert("status", status);

    return obj;
}

QJsonObject GroupModule::ReUpDateGroupName(QJsonObject &data)
{
    uint groupNo      = (uint)data.value("groupNo").toInt();
    QString groupName = data.value("groupName").toString();
    bool status       = UpDateGroupName(groupNo, groupName);

    QJsonObject obj;
    obj.insert("response", "reUpdateGroupName");
    obj.insert("status", status);

    return obj;
}

QJsonObject GroupModule::ReAddDevToGroup(QJsonObject &data)
{
    uint groupNo = (uint)data.value("groupNo").toInt();
    uint devNo   = (uint)data.value("devNo").toInt();

    bool status = InsertDevToGroup(devNo, groupNo);
    if (status) {
        na_add_dev_to_group(groupNo, devNo);
    }

    QJsonObject obj;
    obj.insert("response", "reAddDevToGroup");
    obj.insert("status", status);

    return obj;
}

QJsonObject GroupModule::ReDeleteDevToGroup(QJsonObject &data)
{
    uint devNo   = (uint)data.value("devNo").toInt();
    uint groupNo = (uint)data.value("groupNo").toInt();

    bool status = DeleteDevToGroup(devNo);
    if (status) {
        na_add_dev_to_group(groupNo, devNo);
    }

    QJsonObject obj;
    obj.insert("response", "reDeleteDevToGroup");
    obj.insert("status", status);

    return obj;
}

QJsonObject GroupModule::ReModifyDevToGroup(QJsonObject &data)
{
    uint groupNo = (uint)data.value("groupNo").toInt();
    uint devNo   = (uint)data.value("devNo").toInt();

    bool status = DeleteDevToGroup(devNo) && InsertDevToGroup(devNo, groupNo);

    QJsonObject obj;
    obj.insert("response", "reModifyDevToGroup");
    obj.insert("status", status);

    return obj;
}

QJsonObject GroupModule::ReInquireDevToGroup(QJsonObject &data)
{
    QString devName = data.value("devName").toString();
    uint devNo      = (uint)data.value("devNo").toInt();
    uint groupNo    = (uint)data.value("groupNo").toInt();

    QJsonObject obj;
    obj.insert("response", "reInquireDevToGroup");
    obj.insert("groupNo", (int)groupNo);

    QVector<Device> devs = SelectGpDevs(groupNo, devNo, devName);
    QJsonArray devList;
    for (auto dev : devs) {
        na_get_device_status(dev.devNo, &dev.devStatus);

        QJsonObject devObj;
        devObj.insert("devNo", (int)dev.devNo);
        devObj.insert("devName", dev.devName);
        devObj.insert("devStatus", (dev.devStatus == DEVS_OFFLINE));
        devObj.insert("devVolume", dev.volume);

        devList.append(devObj);
    }
    obj.insert("devList", devList);

    return obj;
}
