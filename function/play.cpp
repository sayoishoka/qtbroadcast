 #include "play.h"
#include "dbinteraction.h"
#include "device.h"
#include "connect/dispatcher.h"
#include "qjsonobject.h"
#include "qsqlquery.h"

#include <lib/NAudioServerLib.h>
#include <QDebug>
#include <QFile>
#include <QJsonArray>
#include <QJsonObject>
#include <QSqlError>
#include <QSqlQuery>
#include <QString>
#include <QTextCodec>
#include <QTimer>
#include <QVector>
#include <cstring>

PlayModule *PlayModule::getInstance()
{
    static PlayModule playModule;
    return &playModule;
}

PlayModule::PlayModule(QObject *parent)
    : QObject(parent)
{
    InitMapper();

    // 设置事实节目处理
    m_playTimer = new QTimer();
    m_playTimer->setInterval(1000);
    connect(m_playTimer, &QTimer::timeout, this, &PlayModule::CheckPlay);
    m_playTimer->start();

    // 设置定时节目处理
    m_timingPmTimer = new QTimer();
    m_timingPmTimer->setInterval(1000);
    connect(m_timingPmTimer, &QTimer::timeout, this, &PlayModule::CheckTimingPlay);
    m_timingPmTimer->start();
    DateUpdate();  // 第一次启动刷新列表
}



void PlayModule::InitMapper()
{
    using namespace std::placeholders;
    // 类型别名
    using Func = std::function<QJsonObject(QJsonObject &)>;
    // 引用分发器的指针
    Dispatcher *dispatcher = Dispatcher::getDispatcher();

    Func rePlayProgram = std::bind(&PlayModule::RePlayProgram, this, _1);
    dispatcher->Register("playProgram", rePlayProgram);

    Func reStopPlayProgram = std::bind(&PlayModule::ReStopPlayProgram, this, _1);
    dispatcher->Register("stopPlayProgram", reStopPlayProgram);

    Func rePausePlayProgram = std::bind(&PlayModule::RePausePlayProgram, this, _1);
    dispatcher->Register("pausePlayProgram", rePausePlayProgram);

    Func reReStorePlayProgram = std::bind(&PlayModule::ReRestorePlayProgram, this, _1);
    dispatcher->Register("restorePlayProgram", reReStorePlayProgram);

    Func reChangePlayFile = std::bind(&PlayModule::changePlayFile, this, _1);
    dispatcher->Register("changePlayFile", reChangePlayFile);

    Func reStartPlayInRandom = std::bind(&PlayModule::startPlayInRandom, this, _1);
    dispatcher->Register("startPlayInRandom", reStartPlayInRandom);

    Func reStartCycle = std::bind(&PlayModule::startCycle, this, _1);
    dispatcher->Register("startCycle", reStartCycle);

    Func reStartPlayInOrder = std::bind(&PlayModule::startPlayInOrder, this, _1);
    dispatcher->Register("startPlayInOrder", reStartPlayInOrder);

    Func reGetDevsUsing = std::bind(&PlayModule::getDevsUsing, this, _1);
    dispatcher->Register("getDevsUsing", reGetDevsUsing);

    Func reGetDevPromMessage = std::bind(&PlayModule::getDevPromMessage , this, _1);
    dispatcher->Register("getDevPromMessage", reGetDevPromMessage);


}


void PlayModule::CheckPlay()
{
    // 已经播放完成的节目
    QVector<unsigned char> eraseList;

    for(auto &it : m_playCache) {
        _play_info playInfo;             // 播放信息
        unsigned char pmNo = it.first;   // 节目号
        pm_info &pmInfo    = it.second;  // 节目信息
        unsigned char scene{};           // 播放场景
        if(pmInfo.playType == ".mp3")
            scene = SCENE_PLAY_AUDIOFILE;
        else if(pmInfo.playType == ".txt")
            scene = SCENE_PLAY_TTS;

        // 获取节目状态
        na_get_playinfo(scene, pmNo, &playInfo);

        if(playInfo.playstatus == PS_STOP) {  // 节目处于停止状态
            // 所有文件播放结束
            int &index = pmInfo.currentFileIndex;  // 引用当前下标

            if(pmInfo.playway == 1){//播放方式是列表播放
                if(pmInfo.currentFileIndex == pmInfo.fileList.size() - 1) {
                    pmInfo.currentFileIndex = -1;      // 重置播放状态
                }
                index++;                               // 切换下一首
            }

            //播放方式是循环播放则播放下标不变

            if(scene == SCENE_PLAY_AUDIOFILE) {    // 音频文件
                const char *filePath = pmInfo.fileList[index].filePath.toStdString().data();
                char *filePath1 = strdup(filePath);
                na_start_play(scene, pmNo, filePath1, 0, pmInfo.devList, pmInfo.devcnt);
                free(filePath1);
            }
            else if(scene == SCENE_PLAY_TTS) {  // tts播放
                // 读取文件
                QFile file(pmInfo.fileList[index].filePath);
                if(!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
                    qDebug() << "Can't open the tts file!";
                }
                QString txt = file.readAll();
                // 设置GBK编码
                QTextCodec *gbk     = QTextCodec::codecForName("gbk");
                QByteArray gb_bytes = gbk->fromUnicode(txt);
                char *txtData       = gb_bytes.data();
                na_start_play(scene, pmNo, txtData, 0, pmInfo.devList, pmInfo.devcnt);
            }

            // na_set_play_volume(scene, pmNo, 50);  // 测试时调小音量
            for(int i = 0; i < pmInfo.devcnt; i++) {
                unsigned char volume = DeviceModule::getInstance()->GetVolume(pmInfo.devList[i]);
                na_add_play_device(scene, pmNo, pmInfo.devList[i]);
                na_set_device_volume(pmInfo.devList[i], volume);
            }
        }
        else if(playInfo.playstatus == PS_PAUSE) {  // 节目处于暂停状态
            /* 略过... */
        }
        else if(playInfo.playstatus == PS_PLAYING) {  // 节目处于播放状态
            // 如果为实时节目,返回播放信息
            if(pmInfo.pmType == REAL_TIME) {
                int progress  = playInfo.progress;
                int totaltime = playInfo.totaltime;
                QJsonObject obj;
                obj.insert("response", "rePlayInfo");
                obj.insert("progress", progress);
                obj.insert("pmNo", pmNo);
                obj.insert("totaltime", totaltime);
                // 广播播放信息
                // Server::GetServer()->Broadcast(obj);

            }

            pmInfo.curTime++;
            if(pmInfo.totTime == pmInfo.curTime) {
                eraseList.push_back(pmNo);
            }
        }
    }

    // 删除已经完成播放的节目
    for(auto no : eraseList) {
        for(int i=0;i<m_playCache[no].devcnt;i++){
            devNo_pmNo.erase (m_playCache[no].devList[i]);
        }
        m_playCache.erase(no);
    }
}

void PlayModule::DateUpdate()
{
    // 清理上一天的节目
    m_todayPargrams.clear();
    int toDay = QDate::currentDate().dayOfWeek() - 1;

    QVector<std::tuple<int, int, QTime>> pmList = SelectCurDatePm();
    for(const auto &pmInfo : pmList) {
        int pmNo        = std::get<0>(pmInfo);
        int weekCnt     = std::get<1>(pmInfo);
        QTime startTime = std::get<2>(pmInfo);

        // 今天为设定的工作日
        if((1 << toDay) & weekCnt) {
            m_todayPargrams.push_back({pmNo, startTime});
        }
    }
}

void PlayModule::CheckTimingPlay()
{
    QTime curTime = QTime::currentTime();  // 获取当前时间
    // 如果为一天的开始更新当日播放的数据
    if(curTime.toString("hh:mm:ss") == QString("00:00:00")) {
        DateUpdate();
    }

    for(const auto &pmInfo : m_todayPargrams) {
        const QTime &pmTime = std::get<1>(pmInfo);
        if(pmTime.toString("hh:mm:ss") == curTime.toString("hh:mm:ss")) {
            int pmNo = std::get<0>(pmInfo);
            // 获取播放设备和文件列表
            QVector<FileInfo> playFileList = SelectPmFile(pmNo);
            QVector<Device> devList        = SelectPlayDev(pmNo);
            // 设置播放信息
            pm_info pmInfo{};
            pmInfo.pmType           = TIMING;
            pmInfo.devcnt           = 0;
            pmInfo.fileList         = playFileList;
            pmInfo.totTime          = 0;
            pmInfo.curTime          = 0;
            pmInfo.currentFileIndex = -1;
            pmInfo.playway          = 1;
            // 获取目前空闲的设备
            for(auto dev : devList) {
                unsigned short devStatus;
                na_get_device_status(dev.devNo, &devStatus);
                if(devStatus == DEVS_IDLE) {
                    pmInfo.devList[pmInfo.devcnt++] = dev.devNo;
                    devNo_pmNo.insert ({dev.devNo,pmNo});
                }
            }
            m_playCache.insert({pmNo, pmInfo});
        }
    }
}

/* 响应接口 */
QJsonObject PlayModule::RePlayProgram(QJsonObject &data)
{
    int pmNo = data.value("pmNo").toInt();



    // 节目信息设置
    Program program                = SelectProgram(pmNo);  // 节目信息
    QVector<Device> devList        = SelectPlayDev(pmNo);  // 播放设备
    QVector<FileInfo> playFileList = SelectPmFile(pmNo);   // 获取文件列表

    // 设置播放信息
    pm_info pmInfo{};
    pmInfo.playType         = program.playType;
    pmInfo.pmType           = REAL_TIME;
    pmInfo.devcnt           = 0;
    pmInfo.fileList         = playFileList;
    pmInfo.totTime          = program.duration.msecsSinceStartOfDay() / 1000;  // 设置的播放时间
    pmInfo.curTime          = 0;
    pmInfo.currentFileIndex = -1;
    pmInfo.playway          = 1;                 //默认循环播放

    int status= 1;
    // 获取目前空闲的设备
    for(auto dev : devList) {
        unsigned short devStatus;
        int flag = 0 ;

        m_rwLock.lockForRead();
        na_get_device_status(dev.devNo, &devStatus);
        m_rwLock.unlock();

        if(devStatus == DEVS_IDLE) {
            flag = 1;
            pmInfo.devList[pmInfo.devcnt++] = dev.devNo;
            devNo_pmNo.insert ({dev.devNo,pmNo});
        }
        if(!flag)status = 0;
    }
    int * volume1;
    // na_get_device_volume(,volume1;);

    m_rwLock.lockForWrite();
    m_playCache.insert({pmNo, pmInfo});
    m_rwLock.unlock();
    QJsonObject obj;
    obj.insert("response", "rePlayProgram");
    obj.insert("status", status);
    return obj;
}

QJsonObject PlayModule::ReStopPlayProgram(QJsonObject &data)
{
    unsigned char pmNo = (unsigned char)data.value("pmNo").toInt();
    for(int i=0;i<m_playCache[pmNo].devcnt;i++){
            devNo_pmNo.erase (m_playCache[pmNo].devList[i]);
    }
    m_playCache.erase(pmNo);


    m_rwLock.lockForWrite();
    bool status = !na_stop_play(SCENE_PLAY_AUDIOFILE, pmNo);
    m_rwLock.unlock();

    QJsonObject obj;
    obj.insert("response", "reStopPlayProgram");
    obj.insert("status", status);

    return obj;
}

QJsonObject PlayModule::RePausePlayProgram(QJsonObject &data)
{
    unsigned char pmNo = (unsigned char)data.value("pmNo").toInt();

    m_rwLock.lockForWrite();
    bool status = !na_pause_play(SCENE_PLAY_AUDIOFILE, pmNo);
    m_rwLock.unlock();

    QJsonObject obj;
    obj.insert("response", "rePausePlayProgram");
    obj.insert("status", status);

    return obj;
}

QJsonObject PlayModule::ReRestorePlayProgram(QJsonObject &data)
{
    unsigned char pmNo = (unsigned char)data.value("pmNo").toInt();
    qDebug() << pmNo;

    m_rwLock.lockForWrite();
    bool status = !na_restore_play(SCENE_PLAY_AUDIOFILE, pmNo);
    m_rwLock.unlock();

    QJsonObject obj;
    obj.insert("response", "reRestorePlayProgram");
    obj.insert("status", status);

    return obj;
}

QJsonObject PlayModule::changePlayFile(QJsonObject &data){

    unsigned char pmNo = data.value("pmNo").toInt();

    unsigned char scene{};

    int currentFileNo  = data.value("fileNo").toInt();


    bool status = 0;

    for(auto &it : m_playCache) {
        _play_info playInfo;             // 播放信息
        unsigned char pmNo1 = it.first;   // 节目号

        if(pmNo1 != pmNo)continue;

        status = 1;

        pm_info &pmInfo    = it.second;  // 节目信息
                   // 播放场景
        if(pmInfo.playType == ".mp3")
            scene = SCENE_PLAY_AUDIOFILE;
        else if(pmInfo.playType == ".txt")
            scene = SCENE_PLAY_TTS;

        int index=pmInfo.currentFileIndex;
        for(int i =0 ;i<pmInfo.fileList.size();i++){
            qDebug()<<"fileNO"<<pmInfo.fileList[i].fileNo;
        }

        qDebug()<<"index"<<index;

        if(data.value("changeway").toString()=="lastfile"){
            if(index ==0){
                index = pmInfo.fileList.size() - 1;
            }
            else index--;
        }
        else if(data.value("changeway").toString()=="nextfile"){
            if(index == pmInfo.fileList.size() - 1)index = 0;
            else index++;
        }
        else{
            for(int i=0;i<=pmInfo.fileList.size() - 1;i++){
                if(pmInfo.fileList[i].fileNo == currentFileNo){
                    index = i;
                    break;
                }
            }
        }
        qDebug()<<"index1"<<index;
        const char *filePath = pmInfo.fileList[index].filePath.toStdString().data();
        char *filePath1 = strdup(filePath);
        pmInfo.currentFileIndex = index;
        currentFileNo = pmInfo.fileList[index].fileNo;
        na_stop_play(SCENE_PLAY_AUDIOFILE, pmNo);
        na_start_play(scene, pmNo, filePath1, 0, pmInfo.devList, pmInfo.devcnt);

        break;

    }

    QJsonObject obj;
    obj.insert("response", "reChangePlayFile");
    obj.insert("currentFileNo", currentFileNo);
    obj.insert("status", status);

    return obj;


}

QJsonObject PlayModule::startCycle(QJsonObject &data){

    unsigned char pmNo = data.value("pmNo").toInt();
    bool status = 0;
    for(auto &it : m_playCache) {
        _play_info playInfo;             // 播放信息
        unsigned char pmNo1 = it.first;   // 节目号

        if(pmNo1 != pmNo)continue;

        status = 1;
        pm_info &pmInfo    = it.second;
        pmInfo.playway = 0;

    }



    QJsonObject obj;
    obj.insert("response", "reStartCycle");
    obj.insert("status", status);

    return obj;

}

QJsonObject PlayModule::startPlayInOrder(QJsonObject &data){


    unsigned char pmNo = data.value("pmNo").toInt();
    bool status = 0;
    for(auto &it : m_playCache) {
        _play_info playInfo;             // 播放信息
        unsigned char pmNo1 = it.first;   // 节目号

        if(pmNo1 != pmNo)continue;

        status = 1;
        pm_info &pmInfo    = it.second;
        pmInfo.playway = 1;

    }

    QJsonObject obj;
    obj.insert("response", "reStartcycle");
    obj.insert("status", status);

    return obj;
}




QJsonObject PlayModule::startPlayInRandom(QJsonObject &data){
    unsigned char pmNo = data.value("pmNo").toInt();
    bool status = 0;
    for(auto &it : m_playCache) {
        _play_info playInfo;             // 播放信息
        unsigned char pmNo1 = it.first;   // 节目号

        if(pmNo1 != pmNo)continue;

        status = 1;
        pm_info &pmInfo    = it.second;
        std::random_shuffle(pmInfo.fileList.begin(), pmInfo.fileList.end()); // 随机播放文件

        // 打印文件顺序
        qDebug() << "File order after shuffle:";
        for(const auto &file : qAsConst(pmInfo.fileList)) {
            qDebug() << file.fileName;
        }
    }

    QJsonObject obj;
    obj.insert("response", "reRandomPlay");
    obj.insert("status", status);

    return obj;
}

QJsonObject PlayModule::getDevsUsing(QJsonObject &data){
    QVector<Group> groups = GetAllGroup();
    QJsonArray groupLists;
    bool status1 = false;
    for(int i=0;i<groups.size ();i++){
        QJsonArray devLists;
        QVector<Device> devices = SelectGpDevs(groups[i].groupNo);
        for(int j=0;j<devices.size ();j++){
            unsigned short status;
            na_get_device_status(devices[j].devNo,&status);
            if(status==DEVS_PLAYING){
                QJsonObject devInfo;
                devInfo["devName"] = devices[j].devName;
                devInfo["devNo"] = (int)devices[j].devNo;
                devLists.append(devInfo);
                status1 = true;
            }
        }
        if(!devLists.isEmpty()){
            QJsonObject groupInfo;
            groupInfo["groupName"] = groups[i].groupName;
            groupInfo["groupNo"] = (int)groups[i].groupNo;
            groupInfo["devLists"] = devLists;
            groupLists.append(groupInfo);
        }
    }
    QJsonObject obj;
    obj.insert("response", "reGetDevsUsing");
    obj.insert ("devLists", groupLists);
    obj.insert("status", status1);
    return obj;
}


QJsonObject PlayModule::getDevPromMessage(QJsonObject &data){
    // 从 data 中提取设备编号
    int devNo = data["devNo"].toInt();

    // 使用 devNo_pmNo 映射获取节目编号
    auto it = devNo_pmNo.find(devNo);
    if (it == devNo_pmNo.end()) {
        // 如果在 devNo_pmNo 中找不到 devNo，返回错误信息
        return QJsonObject{
            {"response", "reGetDevPromMessage"},
            {"status", false},
            {"message", "设备编号不存在"}
        };
    }

    int pmNo = it->second;

    // 使用 pmNo 从 m_playCache 中获取节目信息
    pm_info info = m_playCache[pmNo];

    QString filename = info.fileList[info.currentFileIndex].fileName;
    int     fileNo   = info.fileList[info.currentFileIndex].fileNo;

    // 创建一个 QJsonArray 来存储文件列表
    QJsonArray fileList;
    for (const FileInfo &file : info.fileList) {
        QJsonObject fileObj;
        fileObj["fileName"] = file.fileName;
        fileObj["fileNo"] = file.fileNo;
        fileObj["fileType"] = file.fileType;
        fileList.append(fileObj);
    }

    // 创建一个 QJsonObject 来存储响应数据
    QJsonObject response;
    response["response"] = "reGetDevPromMessage";
    response["pomName"] = SelectProgram (pmNo).pmName+" "+ filename;
    response["currentPomNo"] = pmNo;
    response["currentFileNo"] = fileNo;
    response["fileList"] = fileList;
    response["status"] = true;

    return response;
}
