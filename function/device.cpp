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
    Func reGroupDevs = std::bind(&DeviceModule::ReGroupDevs, this, _1);
    dispatcher->Register("getGroupDevs", reGroupDevs);

    Func reAddDevice = std::bind(&DeviceModule::ReAddDevice, this, _1);
    dispatcher->Register("addDevice", reAddDevice);

    Func reSetDevVolume = std::bind(&DeviceModule::ReSetDevVolume, this, _1);
    dispatcher->Register("setDevVolume",reSetDevVolume);
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
