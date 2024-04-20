#include "file.h"
#include "dbinteraction.h"
#include "connect/dispatcher.h"

#include <QDate>
#include <QFile>
#include <QJsonArray>
#include <QJsonObject>
#include <QSqlError>
#include <QSqlQuery>
#include <QTcpServer>
#include <QTcpSocket>
#include <functional>
#include<QDir>
FileModule *FileModule::getInstance()
{
    static FileModule fileModule;
    return &fileModule;
}

FileModule::FileModule(QObject *parent)
    : QObject(parent)
{
    InitMapper();
}


void FileModule::InitMapper()
{
    using namespace std::placeholders;
    // 类型别名
    using Func = std::function<QJsonObject(QJsonObject &)>;
    // 引用分发器的指针
    Dispatcher *dispatcher = Dispatcher::getDispatcher();

    Func reFileUpload = std::bind(&FileModule::ReFileUpload, this, _1);
    dispatcher->Register("fileUpload", reFileUpload);

    Func reGetFileList = std::bind(&FileModule::ReGetFileList, this, _1);
    dispatcher->Register("getFileList", reGetFileList);

    Func reAuditFile = std::bind(&FileModule::ReAuditFile, this, _1);
    dispatcher->Register("auditFile", reAuditFile);

    Func reDeleteFile = std::bind(&FileModule::ReDeleteFile, this, _1);
    dispatcher->Register("deleteFile", reDeleteFile);

    Func reInquireFile = std::bind(&FileModule::ReInquireFile, this, _1);
    dispatcher->Register("inquireFile", reInquireFile);
}


/* 响应接口 */

QJsonObject FileModule::ReFileUpload(QJsonObject &data)
{
    // 文件路径
    QString fileName = data.value("fileName").toString();
    QString fileType = data.value("fileType").toString();
    QString filePath;
    QString dirPath;
    if (fileType == ".mp3") {
        dirPath = "music";
        filePath = QString(dirPath + "/" + fileName + fileType);
    }
    else if (fileType == ".txt") {
        dirPath = "tts";
        filePath = QString(dirPath + "/" + fileName + fileType);
    }

    qDebug()<<filePath;
    // 文件数据
    QString fileData = data.value("data").toString();
    QByteArray byteArray;
    if (fileType == ".mp3")
        byteArray = QByteArray::fromBase64(fileData.toUtf8());
    else if (fileType == ".txt")
        byteArray = fileData.toUtf8();

    // 检查文件夹是否存在，如果不存在，创建一个
    QDir dir(dirPath);
    if (!dir.exists()) {
        dir.mkpath(".");
    }

    // 写入文件
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly)) {
        qDebug() << file.errorString();
        QJsonObject obj;
        return obj;
    }
    file.write(byteArray);
    file.close();

    QJsonObject obj;
    // 向数据库添加记录
    bool status = AddFileRecord(fileName, fileType, filePath);
    obj.insert("response", "reFileUpload");
    obj.insert("status", status);

    return obj;
}

QJsonObject FileModule::ReGetFileList(QJsonObject &data)
{
    QString fileType("");
    if (!data.value("fileType").isUndefined()) {
        fileType = data.value("fileType").toString();
    }
    QVector<FileInfo> fileInfoList = SelectAllFileInfo(fileType);
    QJsonObject obj;
    obj.insert("response", "reGetFileList");
    QJsonArray fileArray;
    for (auto info : fileInfoList) {
        QJsonObject infoObj;
        infoObj.insert("fileName", info.fileName);
        infoObj.insert("fileNo", info.fileNo);
        infoObj.insert("fileType", info.fileType);
        infoObj.insert("upLoadTime", info.uploadTime.toString("yyyy.MM.dd hh:mm:ss"));
        infoObj.insert("auditStatus", info.auditStatus);

        fileArray.push_back(infoObj);
    }
    obj.insert("fileList", fileArray);

    return obj;
}

QJsonObject FileModule::ReAuditFile(QJsonObject &data)
{
    int fileNo       = data.value("fileNo").toInt();
    bool auditResult = data.value("auditResult").toBool();
    bool status      = SetAuditStatus(fileNo, auditResult);

    QJsonObject obj;
    obj.insert("response", "reAuditFile");
    obj.insert("status", status);

    return  obj;
}

QJsonObject FileModule::ReDeleteFile(QJsonObject &data)
{
    int fileNo       = data.value("fileNo").toInt();
    QString filePath = SelectFilePath(fileNo);
    QFile file(filePath);

    QJsonObject obj;
    obj.insert("response", "reDeleteFile");
    bool status = true;

    if (file.exists() && file.remove()) {  // 短路机制
        status = DeleteFileRecord(fileNo);
    }
    else {
        status = DeleteFileRecord(fileNo);
        status = false;
    }
    obj.insert("status", status);
    return obj;
}

QJsonObject FileModule::ReInquireFile(QJsonObject &data)
{
    QString fuzzyName = data.value("fuzzyName").toString();
    int checkStatus  = data.value("checkStatus").toInt();

    auto fileList = SelectFileList(checkStatus, fuzzyName);

    QJsonObject obj;
    obj.insert("response", "reInquireFile");
    obj.insert("checkStatus", checkStatus);
    QJsonArray fileArray;
    for (int i = 0; i < fileList.size(); i++) {
        QJsonObject fileObj;
        fileObj.insert("fileNo", fileList[i].fileNo);
        fileObj.insert("fileName", fileList[i].fileName);
        fileObj.insert("fileType", fileList[i].fileType);
        fileObj.insert("upLoadTime", fileList[i].uploadTime.toString("yyyy.MM.dd hh:mm:ss"));

        fileArray.append(fileObj);
    }
    obj.insert("fileList", fileArray);

    return obj;
}
