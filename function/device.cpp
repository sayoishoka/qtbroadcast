#include "device.h"
#include "connect/tcp_ip/tcplink.h"
#include "function/dbinteraction.h"
#include "connect/dispatcher.h"
#include "qjsondocument.h"

#include <lib/NAudioServerLib.h>
#include <QDebug>
#include <QJsonArray>
#include <QSqlError>
#include <QSqlQuery>
#include<QApplication>
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
    na_set_callback(DeviceStatusChangeCallback,PlayStatusChangeCallback);
    connect(this,&DeviceModule::devStatusChange,this,&DeviceModule::solveDevStatusChange);
}



void DeviceStatusChangeCallback(unsigned int devno, unsigned short status) {

    // QThread *currentThread = QThread::currentThread();
    // QThread *mainThread = QApplication::instance()->thread();
    // if (currentThread != mainThread) {
    //     qDebug() << "This callback is not running on the main thread!";
    //     // 考虑使用信号/槽或QMetaObject::invokeMethod跨线程安全地调用GUI线程上的方法

    // }

    // emit DeviceModule::getInstance ()->devStatusChange (devno,status);

}

void PlayStatusChangeCallback(unsigned char scene, unsigned char inst, unsigned int status){
    // qDebug() << "play:" ;

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
        devObj.insert("devStatus", dev.devStatus);
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

    uint devNo   = getDevNo(data.value("devNo").toString());
    qDebug()<<devNo;

    unsigned char  volume1;
    unsigned char scene = SCENE_PLAY_AUDIOFILE;
    na_set_device_volume(devNo,volume);
    na_get_device_volume(devNo,&volume1);


    qDebug()<<"volume1"<<volume1;

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



void DeviceModule::startStartVolume(){
    QVector<Device> devs = SelectDevs();
    for(auto dev : devs) {
        na_get_device_status(dev.devNo, &dev.devStatus);
        if(dev.devStatus){
            na_set_device_volume(dev.devNo,dev.volume);
        }
    }
}


void DeviceModule::solveDevStatusChange(unsigned int devno, unsigned short status)
{

    int groupNo = getDevGroupNo (devno);
    QJsonObject obj;
    obj.insert("devno", (int)devno);
    obj.insert("status", status);
    obj.insert("groupNo", groupNo);
    qDebug()<<groupNo;
    TcpLink::getTcpLink ()->broadcastMessage (Dispatcher::getDispatcher ()->Dispatch ("getDevsUsing",obj));
    TcpLink::getTcpLink ()->broadcastMessage (Dispatcher::getDispatcher ()->Dispatch ("getGroupDevs",obj));

}
