#ifndef __DAOTYPE_H__
#define __DAOTYPE_H__

#include <QDateTime>
#include <QString>
#include <QTime>
#include <QVector>

/**
 * 定义用于和数据库交互的结构体
 */

// 设备信息
typedef struct _Device
{
    unsigned int devNo;        // 设备编号
    QString devName;           // 设备名称
    unsigned char volume;      // 音量
    unsigned short devStatus;  // 设备状态
} Device;

// 文件信息
typedef struct _FileInfo
{
    int fileNo;            // 文件号
    QString fileName;      // 文件名
    QString fileType;      // 文件类型
    QString filePath;      // 文件路径
    QDateTime uploadTime;  // 上传时间
    bool auditStatus;      // 审核状态
} FileInfo;

// 分组信息
typedef struct _Group
{
    unsigned int groupNo;  // 分组号
    QString groupName;     // 分组名
} Group;

// 节目信息
typedef struct _Program
{
    int pmNo;          // 节目号(播放实例)
    int pmType;        // 节目播放类型
    QString pmName;    // 节目名
    QString playType;  // 播放文件类型
    QTime duration;    // 播放时长
} Program;


typedef struct _User
{
    QString userName;   // 用户名
    QString userPw;     // 用户密码
    bool userStatus;    // 用户状态
    QString roleName;   // 角色名
    int roleNo;         // 角色编号

} User;

#endif
