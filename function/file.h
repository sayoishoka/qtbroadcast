#ifndef __FILE_H__
#define __FILE_H__


#include <QDateTime>
#include <QObject>
class QJsonObject;
class TcpSession;
class QTcpServer;
class QDateTime;

class FileModule : QObject {
    Q_OBJECT
public:
    static FileModule *getInstance();

private:
    explicit FileModule(QObject *parent = nullptr);


    // 功能部分
private:
    void InitMapper();    // 注册功能

    // 响应接口
private:
    QJsonObject ReFileUpload(QJsonObject &data);
    QJsonObject ReGetFileList(QJsonObject &data);
    QJsonObject ReAuditFile(QJsonObject &data);
    QJsonObject ReDeleteFile(QJsonObject &data);
    QJsonObject ReInquireFile(QJsonObject &data);
};

#endif
