#ifndef __PROGRAM_H__
#define __PROGRAM_H__

#include "daotype.h"
#include <QObject>

class TcpSession;
class QJsonObject;

class ProgramModule : public QObject {
    Q_OBJECT
public:
    static ProgramModule *getInstance();

private:
    explicit ProgramModule(QObject *parent = nullptr);

    // 模块功能
private:
    void InitMapper();    // 注册功能

    // 响应接口
private:
    QJsonObject ReAddProgram(QJsonObject &data);
    QJsonObject ReDeleteProgram(QJsonObject &data);
    QJsonObject ReUpdateProgram(QJsonObject &data);
    QJsonObject ReAddPlayFile(QJsonObject &data);
    QJsonObject ReRemovePlayFile(QJsonObject &data);
    QJsonObject ReGetProgramList(QJsonObject &data);
    QJsonObject ReGetProgramFileList(QJsonObject &data);
    QJsonObject ReAddProgramDev(QJsonObject &data);
    QJsonObject ReRemoveProgramDev(QJsonObject &data);
    QJsonObject ReGetTimingPmInfo(QJsonObject &data);
    QJsonObject ReGetPmDevList(QJsonObject &data);
};

#endif
