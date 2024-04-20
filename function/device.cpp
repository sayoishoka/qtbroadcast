#include "device.h"
#include "function/dbinteraction.h"
#include "connect/dispatcher.h"

#include <lib/NAudioServerLib.h>
#include <QDebug>
#include <QJsonArray>
#include <QSqlError>
#include <QSqlQuery>

DeviceModule *DeviceModule::getInstance()
{
    static DeviceModule deviceModule;
    return &deviceModule;
}

/* 设备模块实现 */
DeviceModule::DeviceModule(QObject *parent)
    : QObject(parent)
{
    InitDev();
    InitMappper();
}

void DeviceModule::InitMappper()
{
    using namespace std::placeholders;
    // 类型别名
    using Func = std::function<QJsonObject(QJsonObject &)>;
    // 引用分发器的指针
    Dispatcher *dispatcher = Dispatcher::getDispatcher();

    /* 注册功能 */
    /* 客户端部分 */
    Func reGroupDevs = std::bind(&DeviceModule::ReGroupDevs, this, _1);
    dispatcher->Register("getGroupDevs", reGroupDevs);

    Func reAddDevice = std::bind(&DeviceModule::ReAddDevice, this, _1);
    dispatcher->Register("addDevice", reAddDevice);

    Func reSetDevVolume = std::bind(&DeviceModule::ReSetDevVolume, this, _1);
    dispatcher->Register("setDevVolume",reSetDevVolume);
    /* 小程序端部分 */
    //用户获取音响列表
    Dispatcher::getDispatcher()->Register("getGroupLists",std::bind(&DeviceModule::getGroupLists, this,std::placeholders::_1));
    //修改音响音量
    Dispatcher::getDispatcher()->Register("setDevVolume",std::bind(&DeviceModule::mo_setDevVolume, this,std::placeholders::_1));
    //批量设置设备音量
    Dispatcher::getDispatcher()->Register("setDevsVolume",std::bind(&DeviceModule::mo_setDevsVolume, this,std::placeholders::_1));
    //获取已有分组
    Dispatcher::getDispatcher()->Register("getGroupList",std::bind(&DeviceModule::mo_getGroupList, this,std::placeholders::_1));
    //获取音响分组
    Dispatcher::getDispatcher()->Register("getGroupDevs",std::bind(&DeviceModule::mo_getGroupDevs, this,std::placeholders::_1));
}


void DeviceModule::InitDev()
{
    auto devs = SelectDevs();

    for(auto &dev : devs) {
        na_get_device_status(dev.devNo, &dev.devStatus);
        m_devs[dev.devNo] = dev;
        qDebug() << dev.devNo << " : " << dev.volume << ":" << m_devs[dev.devNo].volume;
    }
}


/* 接口处理 */
QJsonObject DeviceModule::ReGroupDevs(QJsonObject &data)
{
    QJsonObject obj;
    obj.insert("response", "reGetGroupDevs");

    uint groupNo = (uint)data.value("groupNo").toInt();
    obj.insert("groupNo", (int)groupNo);

    QVector<Device> devs = SelectGpDevs(groupNo);
    QJsonArray devList;
    for(auto dev : devs) {
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

QJsonObject DeviceModule::ReAddDevice(QJsonObject &data)
{
    QString devName = data.value("devName").toString();
    uint devNo      = (uint)data.value("devNo").toInt();

    bool status = InsertDev(devNo, devName, 0);

    if(status) {
        Device dev{devNo, devName, 50, false};
        addDev(devNo, dev);
    }

    QJsonObject obj;
    obj.insert("response", "reAddDevice");
    obj.insert("status", status);
    return obj;
}

QJsonObject DeviceModule::ReSetDevVolume(QJsonObject &data)
{
    uchar volume = (uchar)data.value("volume").toInt();
    uint devNo   = (uint)data.value("devNo").toInt();

    bool status = setDevVolume(devNo, volume);
    if(status) {
        setVolume(devNo, volume);
    }

    QJsonObject obj;
    obj.insert("response", "reSetDevVolume");
    obj.insert("status", status);
    return obj;
}

QJsonObject DeviceModule::getGroupLists(QJsonObject &data)
{
    QSqlQuery query;
    QJsonArray grouplist;
    query.exec("SELECT device.dev_no,device.dev_name,device.longitude,device.latitude,dev_rent.rental_status FROM device "
               "LEFT JOIN dev_rent ON device.dev_no=dev_rent.dev_no");
    while(query.next()){
        QJsonObject dev;
        dev.insert("devId",query.value(0).toInt());
        dev.insert("devName",query.value(1).toString());
        dev.insert("longitude",query.value(2).toString());
        dev.insert("latitude",query.value(3).toString());
        dev.insert("currentStatus",query.value(4).toInt());
        grouplist.append(dev);
    }
    return QJsonObject{
     {"code", 200},
     {"message", "成功"},
     {"data", grouplist}
    };
}

QJsonObject DeviceModule::mo_setDevVolume(QJsonObject &data)
{
    QString uid = QString::number(data.value("userId").toInt());
    QString devNo = QString::number(data.value("devNo").toInt());
    QString devVolume = data.value("volume").toString();
    qDebug()<<"devNo"<<devNo;
    qDebug()<<"devVolume"<<devVolume;
    QSqlQuery query;
    QJsonObject datalist;
    query.exec("UPDATE device SET volume = '"+devVolume+"' WHERE dev_no IN ("+devNo+")");
    datalist.insert("devNo",devNo.toInt());
    datalist.insert("volume",devVolume.toInt());
    return QJsonObject{
     {"code", 200},
     {"message", "成功"},
     {"data", datalist}
    };
}

QJsonObject DeviceModule::mo_setDevsVolume(QJsonObject &data)
{
    QString uid = QString::number(data.value("userId").toInt());
    QJsonArray devNos = data.value("devNos").toArray();
    QString devVolume = QString::number(data.value("volume").toInt());
    QString devNoss = "";
    for (int i = 0; i < devNos.size(); ++i) {
        int value = devNos.at(i).toInt();
        if (i == 0){
            devNoss += value;
        }else{
            devNoss += ","+value;
        }
    }
    QSqlQuery query;
    QJsonArray datalist;
    query.exec("UPDATE device SET volume = '"+devVolume+"' WHERE dev_no IN ("+devNoss+")");
    for (int i = 0; i < devNos.size(); ++i){
        QJsonObject dev;
        dev.insert("devNo",devNos.at(i).toInt());
        dev.insert("volume",devVolume.toInt());
        datalist.append(dev);
    }
    return QJsonObject{
     {"code", 200},
     {"message", "成功"},
     {"data", datalist}
    };
}

QJsonObject DeviceModule::mo_getGroupList(QJsonObject &data)
{
    QString groupName = data.value("groupName").toString();
    QString GroupNo = QString::number(data.value("GroupNo").toInt());
    qDebug()<<"groupName:"<<groupName;
    qDebug()<<"GroupNo:"<<GroupNo;
    QSqlQuery query;
    QJsonArray datalist;
    query.exec("SELECT * FROM `group`");
    while(query.next()){
        QJsonObject group;
        group.insert("groupNo",query.value(0).toInt());
        group.insert("groupName",query.value(1).toString());
        datalist.append(group);
        qDebug()<<query.value(0).toInt();
        qDebug()<<query.value(1).toString();
    }
    return QJsonObject{
     {"code", 200},
     {"message", "成功"},
     {"data", datalist}
    };
}

QJsonObject DeviceModule::mo_getGroupDevs(QJsonObject &data)
{
    QString uid = QString::number(data.value("userId").toInt());
    QString groupNo = data.value("groupNo").toString();
    QSqlQuery query;
    qDebug()<<"groupNo"<<groupNo;
    QJsonArray devlist;
    query.exec("SELECT `group`.gp_no,device.dev_no,device.dev_name,device.volume,device.dev_status FROM `group` "
               "LEFT JOIN `group_info` ON `group`.gp_no=`group_info`.gp_no "
               "LEFT JOIN device ON device.dev_no=`group_info`.dev_no "
               "WHERE `group`.gp_no = "+groupNo);
    while(query.next()){
        QJsonObject dev;
        dev.insert("devNo",query.value(1).toInt());
        dev.insert("devName",query.value(2).toString());
        if (query.value(4).toInt()!=0){
            dev.insert("devStatus",true);
        }else{
            dev.insert("devStatus",false);
        }
        dev.insert("devVolume",query.value(3).toInt());
        devlist.append(dev);
    }
    QJsonObject datalist = QJsonObject{
    {"groupNo", groupNo.toInt()},
    {"devList", devlist}
   };
    return QJsonObject{
     {"code", 200},
     {"message", "成功"},
     {"data", datalist}
    };
}

unsigned char DeviceModule::GetVolume(unsigned int devNo)
{   
    // 添加读锁
    m_rwLock.lockForRead();
    auto volume = m_devs[devNo].volume;
    m_rwLock.unlock();
    return volume;
}

void DeviceModule::addDev(unsigned int devNo, Device dev)
{
    // 添加写锁
    m_rwLock.lockForWrite();
    m_devs[devNo] = dev;
    m_rwLock.unlock();
}

void DeviceModule::setVolume(unsigned int devNo, unsigned char volume)
{
    // 添加写锁
    m_rwLock.lockForWrite();
    m_devs[devNo].volume = volume;
    m_rwLock.unlock();
}
