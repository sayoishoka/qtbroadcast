#ifndef __GROUP_H__
#define __GROUP_H__

#include "daotype.h"

#include <QObject>

class TcpSession;

class GroupModule : public QObject {
    Q_OBJECT
public:
    static GroupModule *getInstance();

private:
    explicit GroupModule(QObject *parent = nullptr);

    // 功能部分
private:
    void InitMapper();    // 注册功能

    /* 接口处理 */
    QJsonObject ReGroupList(QJsonObject &data);         // 返回所有的分组消息
    QJsonObject ReAddGroup(QJsonObject &data);          // 添加分组
    QJsonObject ReDeleteGroup(QJsonObject &data);       // 删除分组
    QJsonObject ReUpDateGroupName(QJsonObject &data);   // 修改分组名称
    QJsonObject ReAddDevToGroup(QJsonObject &data);     // 添加分组中的设备
    QJsonObject ReDeleteDevToGroup(QJsonObject &data);  // 删除分组中的设备
    QJsonObject ReModifyDevToGroup(QJsonObject &data);  // 修改设备到分组
    QJsonObject ReInquireDevToGroup(QJsonObject &data); // 查询分组下设备
};

#endif
