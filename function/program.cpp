#include "program.h"
#include "connect/dispatcher.h"
#include "dbinteraction.h"
#include "connect/dispatcher.h"

#include <lib/NAudioServerLib.h>
#include <QDebug>
#include <QJsonArray>
#include <QJsonObject>
#include <QSqlDataBase>
#include <QSqlDriver>
#include <QSqlError>
#include <QSqlQuery>
#include <QTime>

ProgramModule *ProgramModule::getInstance()
{
    static ProgramModule programModule;
    return &programModule;
}

ProgramModule::ProgramModule(QObject *parent)
    : QObject(parent)
{
    InitMapper();
}



void ProgramModule::InitMapper()
{
    using namespace std::placeholders;
    // 类型别名
    using Func = std::function<QJsonObject(QJsonObject &)>;
    // 引用分发器的指针
    Dispatcher *dispatcher = Dispatcher::getDispatcher();

    Func reAddProgram = std::bind(&ProgramModule::ReAddProgram, this, _1);
    dispatcher->Register("addProgram", reAddProgram);

    Func reDeleteProgram = std::bind(&ProgramModule::ReDeleteProgram, this, _1);
    dispatcher->Register("deleteProgram", reDeleteProgram);

    Func reAddPlayFile = std::bind(&ProgramModule::ReAddPlayFile, this, _1);
    dispatcher->Register("addPlayFile", reAddPlayFile);

    Func reRemovePlayFile = std::bind(&ProgramModule::ReRemovePlayFile, this, _1);
    dispatcher->Register("removePlayFile", reRemovePlayFile);

    Func reGetProgramList = std::bind(&ProgramModule::ReGetProgramList, this, _1);
    dispatcher->Register("getProgramList", reGetProgramList);

    Func reGetProgramFileList = std::bind(&ProgramModule::ReGetProgramFileList, this, _1);
    dispatcher->Register("getProgramFileList", reGetProgramFileList);

    Func reAddProgramDev = std::bind(&ProgramModule::ReAddProgramDev, this, _1);
    dispatcher->Register("addProgramDev", reAddProgramDev);

    Func reRemoveProgramDev = std::bind(&ProgramModule::ReRemoveProgramDev, this, _1);
    dispatcher->Register("removeProgramDev", reRemoveProgramDev);

    Func reUpdateProgram = std::bind(&ProgramModule::ReUpdateProgram, this, _1);
    dispatcher->Register("updateProgram", reUpdateProgram);

    Func reGetTimingPmInfo = std::bind(&ProgramModule::ReGetTimingPmInfo, this, _1);
    dispatcher->Register("getTimingPmInfo", reGetTimingPmInfo);

    Func reGetPmDevList = std::bind(&ProgramModule::ReGetPmDevList, this, _1);
    dispatcher->Register("getPmDevList", reGetPmDevList);
}


/* 响应接口 */
QJsonObject ProgramModule::ReAddProgram(QJsonObject &data)
{
    int pmType       = data.value("pmType").toInt();
    int pmNo         = data.value("pmNo").toInt();
    QString pmName   = data.value("pmName").toString();
    QString playType = data.value("playType").toString();
    QTime duration   = QTime::fromString(data.value("playDuration").toString(), "hh:mm:ss");

    // 写入数据库
    bool status = AddProgram(pmNo, pmName, pmType, playType, duration);

    // 添加定时节目信息
    if (pmType == 1) {
        int weekCnt     = data.value("weekCnt").toInt();
        QDate startDate = QDate::fromString(data.value("startDate").toString(), "yyyy-MM-dd");
        QDate endDate   = QDate::fromString(data.value("endDate").toString(), "yyyy-MM-dd");
        QTime startTime = QTime::fromString(data.value("startTime").toString(), "hh:mm:ss");
        // 写入数据库
        status |= AddTimingInfo(pmNo, startDate, endDate, startTime, weekCnt);
    }

    QJsonObject obj;
    obj.insert("response", "reAddprogram");
    obj.insert("status", status);
    return  obj;
}

QJsonObject ProgramModule::ReDeleteProgram(QJsonObject &data)
{
    int pmNo    = data.value("pmNo").toInt();
    bool status = DeleteProgram(pmNo);

    QJsonObject obj;
    obj.insert("response", "reDeleteProgram");
    obj.insert("status", status);

    return obj;
}

QJsonObject ProgramModule::ReUpdateProgram(QJsonObject &data)
{
    int pmType       = data.value("pmType").toInt();
    int oldPmNo      = data.value("oldPmNo").toInt();
    int newPmNo      = data.value("newPmNo").toInt();
    QString pmName   = data.value("pmName").toString();
    QString playType = data.value("playType").toString();
    QTime duration   = QTime::fromString(data.value("playDuration").toString(), "hh:mm:ss");

    // 写入数据库
    bool status = UpdateProgram(oldPmNo, newPmNo,pmType, pmName, playType, duration);

    // 添加定时节目信息
    if (pmType == 1) {
        qDebug()<<"**";
        int weekCnt     = data.value("weekCnt").toInt();
        QDate startDate = QDate::fromString(data.value("startDate").toString(), "yyyy-MM-dd");
        QDate endDate   = QDate::fromString(data.value("endDate").toString(), "yyyy-MM-dd");
        QTime startTime = QTime::fromString(data.value("startTime").toString(), "hh:mm:ss");
        // 写入数据库
        if(SelectTimeingInfo (newPmNo))
            status |= UpdateTimingInfo(newPmNo, startDate, endDate, startTime, weekCnt);
        else
            status |= AddTimingInfo(newPmNo, startDate, endDate, startTime, weekCnt);
    }

    QJsonObject obj;
    obj.insert("response", "reUpdateProgram");
    obj.insert("status", status);
    return obj;
}

QJsonObject ProgramModule::ReAddPlayFile(QJsonObject &data)
{
    int pmNo     = data.value("pmNo").toInt();
    int fileNo   = data.value("fileNo").toInt();
    int priority = data.value("priority").toInt();
    bool status  = AddFileToProgram(pmNo, fileNo, priority);

    QJsonObject obj;
    obj.insert("response", "reAddPlayFile");
    obj.insert("status", status);

    return obj;
}

QJsonObject ProgramModule::ReRemovePlayFile(QJsonObject &data)
{
    int pmNo    = data.value("pmNo").toInt();
    int fileNo  = data.value("fileNo").toInt();
    bool status = RemovePlayFile(pmNo, fileNo);

    QJsonObject obj;
    obj.insert("response", "reRemovePlayFile");
    obj.insert("status", status);

    return obj;
}

QJsonObject ProgramModule::ReGetProgramList(QJsonObject &data)
{
    // 需要模糊查询的节目名称
    QString pmName = "";
    if (!data.value("pmName").isUndefined()) {
        pmName = data.value("pmName").toString();
    }

    QJsonObject obj;
    obj.insert("response", "reGetProgramList");

    QJsonArray pmObjList;
    QVector<Program> pmList = SelectPrograms(pmName);
    for (auto pm : pmList) {
        QJsonObject pmObj;
        pmObj.insert("pmNo", pm.pmNo);
        pmObj.insert("pmName", pm.pmName);
        pmObj.insert("pmType", pm.pmType);
        pmObj.insert("playDuration", pm.duration.toString("hh:mm:ss"));
        pmObjList.push_back(pmObj);
    }
    obj.insert("programList", pmObjList);

    return obj;
}

QJsonObject ProgramModule::ReGetProgramFileList(QJsonObject &data)
{
    int pmNo = data.value("pmNo").toInt();
    // 查询数据库
    QVector<FileInfo> infoList = SelectPmFile(pmNo);

    QJsonObject obj;
    obj.insert("response", "reGetProgramFileList");
    obj.insert("pmNo", pmNo);

    QJsonArray infoObjList;
    for (auto info : infoList) {
        QJsonObject infoObj;
        infoObj.insert("fileNo", info.fileNo);
        infoObj.insert("fileName", info.fileName);
        infoObj.insert("fileType", info.fileType);
        infoObjList.push_back(infoObj);
    }
    obj.insert("fileList", infoObjList);

    return obj;
}

QJsonObject ProgramModule::ReAddProgramDev(QJsonObject &data)
{
    int devNo = data.value("devNo").toInt();
    int pmNo  = data.value("pmNo").toInt();

    qDebug()<<"devNo"<<devNo<<";"<<"pmNo"<<pmNo;

    bool status = AddProgramDev(pmNo, devNo);

    QJsonObject obj;
    obj.insert("response", "reAddProgramDev");
    obj.insert("status", status);

    return obj;
}

QJsonObject ProgramModule::ReRemoveProgramDev(QJsonObject &data)
{
    int devNo = data.value("devNo").toInt();
    int pmNo  = data.value("pmNo").toInt();

    bool status = RemoveProgramDev(pmNo, devNo);

    QJsonObject obj;
    obj.insert("response", "removeProgramDev");
    obj.insert("status", status);

    return obj;
}

QJsonObject ProgramModule::ReGetTimingPmInfo(QJsonObject &data)
{
    int pmNo = data.value("pmNo").toInt();
    // 查询定时信息
    auto info = SelectTimingInfo(pmNo);
    // 解析定时信息
    QString startDate = std::get<0>(info).toString("yyyy-MM-dd");
    QString endDate   = std::get<1>(info).toString("yyyy-MM-dd");
    QString startTime = std::get<2>(info).toString("hh:mm:ss");
    int weekCnt       = std::get<3>(info);

    QJsonObject obj;
    obj.insert("response", "reGetTimingPmInfo");
    obj.insert("pmNo", pmNo);
    obj.insert("startDate", startDate);
    obj.insert("ednDate", endDate);
    obj.insert("startTime", startTime);
    obj.insert("weekCnt", weekCnt);
    return obj;
}

QJsonObject ProgramModule::ReGetPmDevList(QJsonObject &data)
{
    int pmNo = data.value("pmNo").toInt();

    auto devNoList = SelectPmDevs(pmNo);

    QJsonObject obj;
    obj.insert("response", "reGetPmDevList");
    obj.insert("pmNo", pmNo);
    QJsonArray jsonDevNoList;
    for (int i = 0; i < devNoList.size(); i++) {
        jsonDevNoList.append((int)devNoList[i]);
    }
    obj.insert("devNoList", jsonDevNoList);

    return obj;
}
