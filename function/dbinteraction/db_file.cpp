#include "../dbinteraction.h"

#include <QDebug>
#include <QJsonArray>
#include <QJsonObject>
#include <QSqlError>
#include <QSqlQuery>

bool AddFileRecord(QString &fileName, QString &fileType, QString &filePath)
{
    QSqlQuery query(QSqlDatabase::database(QString::number(reinterpret_cast<quintptr>(QThread::currentThreadId()))));
    query.prepare("insert into `file`(`file_name`, `file_type`, `file_path`, `audit_status`) "
                  "values(:fileName, :fileType, :filePath, false)");
    query.bindValue(":fileName", fileName);
    query.bindValue(":fileType", fileType);
    query.bindValue(":filePath", filePath);

    if (!query.exec()) {
        qDebug() << "exec AddFileRecord failed!!!";
        qDebug() << query.lastError();
        return false;
    }
    else {
        return true;
    }
}

bool SetAuditStatus(int fileNo, bool auditResult)
{
    QSqlQuery query(QSqlDatabase::database(QString::number(reinterpret_cast<quintptr>(QThread::currentThreadId()))));
    query.prepare("update `file` set `audit_status` = :status "
                  "where `file_no` = :fileNo");
    query.bindValue(":status", auditResult);
    query.bindValue(":fileNo", fileNo);

    if (!query.exec()) {
        qDebug() << "exec SetAuditStatus failed!!!";
        qDebug() << query.lastError();
        return false;
    }
    else {
        return true;
    }
}

bool DeleteFileRecord(int fileNo)
{
    QSqlQuery query(QSqlDatabase::database(QString::number(reinterpret_cast<quintptr>(QThread::currentThreadId()))));
    query.prepare("delete from `file` where `file_no` = :fileNo");
    query.bindValue(":fileNo", fileNo);

    if (!query.exec()) {
        qDebug() << "exec DeleteFileRecord failed!!!";
        qDebug() << query.lastError();
        return false;
    }
    else {
        return true;
    }
}

QString SelectFilePath(int fileNo)
{
    QSqlQuery query(QSqlDatabase::database(QString::number(reinterpret_cast<quintptr>(QThread::currentThreadId()))));
    query.prepare("select `file_path` from `file` "
                  "where `file_no` = :fileNo;");
    query.bindValue(":fileNo", fileNo);

    if (!query.exec() || !query.next()) {
        qDebug() << "exec SelectFilePath failed!!!";
        qDebug() << query.lastError();
        return QString();
    }
    else {
        return query.value("file_path").toString();
    }
}

QVector<FileInfo> SelectAllFileInfo(QString &fileType)
{
    QSqlQuery query(QSqlDatabase::database(QString::number(reinterpret_cast<quintptr>(QThread::currentThreadId()))));
    QVector<FileInfo> fileInfoList;
    QString sql = "select * from `file` ";
    if (!fileType.isEmpty()) {
        sql += QString("where `file_type` = '" + fileType + "';");
    }
    if (!query.exec(sql)) {
        qDebug() << "exec SelectAllFileInfo failed!!!";
        qDebug() << query.lastError();
        return fileInfoList;
    }
    else {
        while (query.next()) {
            FileInfo info;
            info.fileNo      = query.value("file_no").toInt();
            info.fileName    = query.value("file_name").toString();
            info.fileType    = query.value("file_type").toString();
            info.uploadTime  = query.value("upload_date").toDateTime();
            info.auditStatus = query.value("audit_status").toBool();
            info.filePath    = query.value("file_path").toString();

            fileInfoList.push_back(info);
        }
        return fileInfoList;
    }
}

QVector<FileInfo> SelectFileList(bool checkStatus, QString fuzzyName)
{
    QVector<FileInfo> fileList;
    QSqlQuery query(QSqlDatabase::database(QString::number(reinterpret_cast<quintptr>(QThread::currentThreadId()))));
    QString sql = "select * from `file` where " +
                  QString("`audit_status` = " + QString(checkStatus ? "true" : "false")) +
                  QString(" and `file_name` like '%" + fuzzyName + "%';");

    if (!query.exec(sql)) {
        qDebug() << "exec SelectFileList failed!";
        qDebug() << query.lastError();
        return fileList;
    }
    else {
        while (query.next()) {
            FileInfo info;
            info.fileNo      = query.value("file_no").toInt();
            info.fileName    = query.value("file_name").toString();
            info.fileType    = query.value("file_type").toString();
            info.uploadTime  = query.value("upload_date").toDateTime();
            info.auditStatus = query.value("audit_status").toBool();
            info.filePath    = query.value("file_path").toString();

            fileList.push_back(info);
        }

        return fileList;
    }
}
