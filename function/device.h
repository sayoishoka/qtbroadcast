#ifndef __DEVICE_H__
#define __DEVICE_H__

#include "daotype.h"
#include "qobjectdefs.h"

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


signals:
    void devStatusChange(unsigned int devno, unsigned short status);

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

    bool isMainThread();
private slots:
    void solveDevStatusChange(unsigned int devno, unsigned short status);


private:
    std::map<unsigned int, Device> m_devs;
    QReadWriteLock m_rwLock;
    void startStartVolume();


};

extern void __stdcall DeviceStatusChangeCallback(unsigned int devno, unsigned short status);
extern void __stdcall PlayStatusChangeCallback(unsigned char scene, unsigned char inst, unsigned int status);
// // 定义播放状态变化回调函数指针类型
// typedef void (*PlayStatusChangeCallback)(unsigned char scene, unsigned char inst, unsigned int status);


#endif
