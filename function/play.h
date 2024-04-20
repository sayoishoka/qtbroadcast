#ifndef __PLAY_H__
#define __PLAY_H__

#include "daotype.h"
#include <QObject>
#include <QString>
#include <map>
#include <QReadWriteLock>
#include<algorithm>
class TcpSession;
class QTimer;

class PlayModule : public QObject {
    Q_OBJECT

public:
    static PlayModule *getInstance();


private:
    explicit PlayModule(QObject *parent = nullptr);
    // 功能模块
private:
    // 节目类型
    enum PM_TYPE {
        REAL_TIME,
        TIMING
    };

    // 节目信息
    typedef struct _pm_info
    {
        QVector<FileInfo> fileList;  // 文件列表
        int currentFileIndex;        // 当前播放到文件下标
        unsigned int devList[255];   // 设备列表
        int devcnt;                  // 设备数目
        int totTime;                 // 节目播放时长
        int curTime;                 // 当前播放时长
        int pmType;                  // 节目类型
        QString playType;            // 播放类型
        int playway;             // 播放方式
    } pm_info;

    std::map<int,int>devNo_pmNo;
    std::map<unsigned char, pm_info> m_playCache;  // 节目缓存
    QTimer *m_playTimer;                           // 检查播放节目
    void CheckPlay();

    // 定时节目
    QVector<std::tuple<int, QTime>> m_todayPargrams;  // 当日节目列表
    QTimer *m_timingPmTimer;                          // 检查定时节目
    void DateUpdate();                                // 刷新当日数据
    void CheckTimingPlay();                           // 检查定时节目

    void InitMapper();                                // 注册功能
    void LogoutMapper();                              // 注销功能

    // 响应接口
private:
    QJsonObject RePlayProgram(QJsonObject &data);
    QJsonObject ReStopPlayProgram(QJsonObject &data);
    QJsonObject RePausePlayProgram(QJsonObject &data);
    QJsonObject ReRestorePlayProgram(QJsonObject &data);
    QJsonObject changePlayFile(QJsonObject &data);
private:
    QReadWriteLock m_rwLock;

    QJsonObject startCycle(QJsonObject &data);
    QJsonObject startPlayInOrder(QJsonObject &data);
    QJsonObject startPlayInRandom(QJsonObject &data);
    QJsonObject getDevsUsing(QJsonObject &data);
    QJsonObject getDevPromMessage(QJsonObject &data);
};

#endif
