#ifndef __DEVICE_H__
#define __DEVICE_H__

#include "daotype.h"

#include <lib/NAudioServerLib.h>
#include <QObject>
#include <QSet>
#include <QReadWriteLock>

class TcpSession;

// 设备模块
class DeviceModule : public QObject {
    Q_OBJECT

public:
    static DeviceModule *getInstance();

private:
    explicit DeviceModule(QObject *parent = nullptr);

    // 功能部分
private:
    void InitMappper();   // 注册功能 // 注销功能
    void InitDev();       // 初始化设备集合
    //void FindNewDev();    // 查找新设备

    /* 接口处理 */
    QJsonObject ReGroupDevs(QJsonObject &data);
    QJsonObject ReAddDevice(QJsonObject &data);
    QJsonObject ReSetDevVolume(QJsonObject &data);
public:
    // 获取设备音量
    unsigned char GetVolume(unsigned int devNo);
    // 添加设备至缓存
    void addDev(unsigned int devNo, Device dev);
    // 设置缓存音量
    void setVolume(unsigned int devNo, unsigned char volume);
private:
    std::map<unsigned int, Device> m_devs;
    QReadWriteLock m_rwLock;
};

#endif
