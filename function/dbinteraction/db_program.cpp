#include "../dbinteraction.h"

#include <QDebug>
#include <QJsonArray>
#include <QJsonObject>
#include <QSqlError>
#include <QSqlQuery>

/** 节目 **/
bool AddProgram(
    int pmNo, QString &pmName, int pmType, QString &playType, QTime &duration)
{
    QSqlQuery query(QSqlDatabase::database(QString::number(reinterpret_cast<quintptr>(QThread::currentThreadId()))));
    query.prepare("insert into`program`(`pm_no`, `pm_name`, `pm_type`, "
                  "`play_type`, `play_duration`) "
                  "values(:pmNo, :pmName, :pmType, :playType, :duration)");
    query.bindValue(":pmNo", pmNo);
    query.bindValue(":pmName", pmName);
    query.bindValue(":pmType", pmType);
    query.bindValue(":playType", playType);
    query.bindValue(":duration", duration);

    if (!query.exec()) {
        qDebug() << "exec AddProgram failed!!!";
        qDebug() << query.lastError();
        return false;
    }
    else {
        return true;
    }
}

bool UpdateProgram(int oldPmNo, int newPmNo, QString &pmName, QString &playType, QTime &duration)
{
    QSqlQuery query(QSqlDatabase::database(QString::number(reinterpret_cast<quintptr>(QThread::currentThreadId()))));
    query.prepare("update `program` set "
                  "`pm_no` = :newPmNo, `pm_name` = :pmName, `play_type` = :playType, `play_duration` = :duration "
                  "where `pm_no` = :oldPmNo;");
    query.bindValue(":newPmNo", newPmNo);
    query.bindValue(":oldPmNo", oldPmNo);
    query.bindValue(":pmName", pmName);
    query.bindValue(":playType", playType);
    query.bindValue(":duration", duration);

    if (!query.exec()) {
        qDebug() << "exec UpdateProgram failed!!!";
        qDebug() << query.lastError();
        return false;
    }
    else {
        return true;
    }
}

bool DeleteProgram(int pmNo)
{
    QSqlQuery query(QSqlDatabase::database(QString::number(reinterpret_cast<quintptr>(QThread::currentThreadId()))));
    query.prepare("delete from `program` where `pm_no` = :pmNo");
    query.bindValue(":pmNo", pmNo);

    if (!query.exec()) {
        qDebug() << "exec DeleteProgram failed!!!";
        qDebug() << query.lastError();
        return false;
    }
    else {
        return true;
    }
}

Program SelectProgram(int pmNo)
{
    Program program{};

    QSqlQuery query(QSqlDatabase::database(QString::number(reinterpret_cast<quintptr>(QThread::currentThreadId()))));
    query.prepare("select * from `program` where `pm_no` = :pmNo; ");
    query.bindValue(":pmNo", pmNo);

    if (!query.exec()) {
        qDebug() << "exec SelectProgram failed!!!";
        qDebug() << query.lastError();
        return program;
    }
    else {
        if (query.next()) {
            program.pmNo     = pmNo;
            program.duration = query.value("play_duration").toTime();
            program.pmName   = query.value("pm_name").toString();
            program.playType = query.value("play_type").toString();
            program.pmType   = query.value("pm_type").toInt();
        }
        return program;
    }
}

QVector<std::tuple<int, int, QTime>> SelectCurDatePm()
{
    QVector<std::tuple<int, int, QTime>> pmList;

    QSqlQuery query(QSqlDatabase::database(QString::number(reinterpret_cast<quintptr>(QThread::currentThreadId()))));
    query.prepare("select * from `timing_info` "
                  "where CURDATE() between `start_date` and `end_date`;");
    if (!query.exec()) {
        qDebug() << "exec SelectCurDatePm failed!!!";
        qDebug() << query.lastError();
        return pmList;
    }
    else {
        while (query.next()) {
            int programNo   = query.value("pm_no").toInt();
            int weekCnt     = query.value("week_cnt").toInt();
            QTime startTime = query.value("start_time").toTime();
            pmList.push_back(std::tuple<int, int, QTime>{programNo, weekCnt, startTime});
        }
        return pmList;
    }
}

QVector<uint> SelectPmDevs(int pmNo)
{
    QVector<uint> devList;

    QSqlQuery query(QSqlDatabase::database(QString::number(reinterpret_cast<quintptr>(QThread::currentThreadId()))));
    query.prepare("select `dev_no` from `pm_dev` where `pm_no` = :pmNo;");
    query.bindValue(":pmNo", pmNo);

    if (!query.exec()) {
        qDebug() << "exec SelectPmDevs failed!!!";
        qDebug() << query.lastError();
        return devList;
    }
    else {
        while (query.next()) {
            uint devNo = (uint)query.value("dev_no").toInt();
            qDebug() << "*****";
            devList.push_back(devNo);
        }
        return devList;
    }
}

QVector<Device> SelectPlayDev(int pmNo)
{
    QVector<Device> devList;
    QSqlQuery query(QSqlDatabase::database(QString::number(reinterpret_cast<quintptr>(QThread::currentThreadId()))));
    query.prepare("select * from `pm_dev` natural join `device` where "
                  "`pm_no` = :pmNo");
    query.bindValue(":pmNo", pmNo);

    if (!query.exec()) {
        qDebug() << "exec SelectPlayFile failed!!!";
        qDebug() << query.lastError();
        return devList;
    }
    else {
        while (query.next()) {
            Device dev;
            dev.devNo   = query.value("dev_no").toUInt();
            dev.devName = query.value("dev_name").toString();
            dev.volume  = query.value("volume").toInt();
            devList.push_back(dev);
        }
        return devList;
    }
}

bool AddFileToProgram(int pmNo, int fileNo, int priority)
{
    QSqlQuery query(QSqlDatabase::database(QString::number(reinterpret_cast<quintptr>(QThread::currentThreadId()))));
    query.prepare("insert into`pm_file`(`pm_no`, `file_no`, `priority`) "
                  "values(:pmNo, :fileNo, :priority)");
    query.bindValue(":pmNo", pmNo);
    query.bindValue(":fileNo", fileNo);
    query.bindValue(":priority", priority);

    if (!query.exec()) {
        qDebug() << "exec AddFileToProgram failed!!!";
        qDebug() << query.lastError();
        return false;
    }
    else {
        return true;
    }
}

bool RemovePlayFile(int pmNo, int fileNo)
{
    QSqlQuery query(QSqlDatabase::database(QString::number(reinterpret_cast<quintptr>(QThread::currentThreadId()))));
    query.prepare("delete from `pm_file` where `pm_no` = :pmNo and `file_no` = :fileNo");
    query.bindValue(":pmNo", pmNo);
    query.bindValue(":fileNo", fileNo);

    if (!query.exec()) {
        qDebug() << "exec RemovePlayFile failed!!!";
        qDebug() << query.lastError();
        return false;
    }
    else {
        return true;
    }
}

bool AddProgramDev(int pmNo, int devNo)
{
    QSqlQuery query(QSqlDatabase::database(QString::number(reinterpret_cast<quintptr>(QThread::currentThreadId()))));
    query.prepare("insert into `pm_dev`(`pm_no`, `dev_no`) values(:pmNo, :devNo)");
    query.bindValue(":pmNo", pmNo);
    query.bindValue(":devNo", devNo);

    if (!query.exec()) {
        qDebug() << "exec AddProgramDev failed!!!";
        qDebug() << query.lastError();
        return false;
    }
    else {
        return true;
    }
}

bool RemoveProgramDev(int pmNo, int devNo)
{
    QSqlQuery query(QSqlDatabase::database(QString::number(reinterpret_cast<quintptr>(QThread::currentThreadId()))));
    query.prepare("delete from `pm_dev` where `pm_no` = :pmNo and `dev_no` = :devNo");
    query.bindValue(":pmNo", pmNo);
    query.bindValue(":devNo", devNo);

    if (!query.exec()) {
        qDebug() << "exec RemoveProgramDev failed!!!";
        qDebug() << query.lastError();
        return false;
    }
    else {
        return true;
    }
}

QVector<Program> SelectPrograms(QString pmName)
{
    QVector<Program> pmList;
    QSqlQuery query(QSqlDatabase::database(QString::number(reinterpret_cast<quintptr>(QThread::currentThreadId()))));
    QString sql = "select * from `program` ";

    if (!pmName.isEmpty()) {
        sql += "where `pm_name` like :pmName ";
        sql.replace(":pmName", "'%" + pmName + "%'");
    }

    if (!query.exec(sql)) {
        qDebug() << "exec SelectPrograms failed!!!";
        qDebug() << query.lastQuery();
        qDebug() << query.lastError();
        return pmList;
    }
    else {
        while (query.next()) {
            Program pm;
            pm.pmNo     = query.value("pm_no").toInt();
            pm.pmName   = query.value("pm_name").toString();
            pm.pmType   = query.value("pm_type").toInt();
            pm.playType = query.value("play_type").toString();
            pm.duration = query.value("play_duration").toTime();
            pmList.push_back(pm);
        }
        return pmList;
    }
}

QVector<FileInfo> SelectPmFile(int pmNo)
{
    QVector<FileInfo> infoList;
    QSqlQuery query(QSqlDatabase::database(QString::number(reinterpret_cast<quintptr>(QThread::currentThreadId()))));
    query.prepare("select * from `file` natural join `pm_file` "
                  "where `pm_no` = :pmNo;");
    query.bindValue(":pmNo", pmNo);

    if (!query.exec()) {
        qDebug() << "exec SelectPmFile failed!!!";
        qDebug() << query.lastError();
        return infoList;
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
            infoList.push_back(info);
        }
        return infoList;
    }
}

bool AddTimingInfo(int pmNo, QDate &startDate, QDate &endDate, QTime &startTime, int weekCnt)
{
    QSqlQuery query(QSqlDatabase::database(QString::number(reinterpret_cast<quintptr>(QThread::currentThreadId()))));
    query.prepare("insert into `timing_info`(`pm_no`, `start_date`, `end_date`, `start_time`, `week_cnt`) "
                  "values(:pmNo, :startDate, :endDate, :startTime, :weekCnt);");
    query.bindValue(":pmNo", pmNo);
    query.bindValue(":startDate", startDate);
    query.bindValue(":endDate", endDate);
    query.bindValue(":startTime", startTime);
    query.bindValue(":weekCnt", weekCnt);

    if (!query.exec()) {
        qDebug() << "exec AddTimingInfo failed!!!";
        qDebug() << query.lastError();
        return false;
    }
    else {
        return true;
    }
}

bool UpdateTimingInfo(int pmNo, QDate &startDate, QDate &endDate, QTime &startTime, int weekCnt)
{
    QSqlQuery query(QSqlDatabase::database(QString::number(reinterpret_cast<quintptr>(QThread::currentThreadId()))));
    query.prepare("update `timing_info` set "
                  "`start_date` = :startDate, `end_date` = :endDate, `start_time` = :startTime, `week_cnt` = :weekCnt "
                  "where `pm_no` = :pmNo");
    query.bindValue(":pmNo", pmNo);
    query.bindValue(":startDate", startDate);
    query.bindValue(":endDate", endDate);
    query.bindValue(":startTime", startTime);
    query.bindValue(":weekCnt", weekCnt);

    if (!query.exec()) {
        qDebug() << "exec UpdateTimingInfo failed!!!";
        qDebug() << query.lastError();
        return false;
    }
    else {
        return true;
    }
}

std::tuple<QDate, QDate, QTime, int> SelectTimingInfo(int pmNo)
{
    // startDate, endDatem, startTime, weekCnt
    std::tuple<QDate, QDate, QTime, int> result;
    QSqlQuery query(QSqlDatabase::database(QString::number(reinterpret_cast<quintptr>(QThread::currentThreadId()))));
    query.prepare("select * from `timing_info` where `pm_no` = :pmNo; ");
    query.bindValue(":pmNo", pmNo);

    if (!query.exec()) {
        qDebug() << "exec UpdateTimingInfo failed!!!";
        qDebug() << query.lastError();
        return result;
    }
    else {
        query.next();
        QDate startDate = query.value("start_date").toDate();
        QDate endDate   = query.value("end_date").toDate();
        QTime startTime = query.value("start_time").toTime();
        int weekCnt     = query.value("week_cnt").toInt();
        return result   = std::tuple<QDate, QDate, QTime, int>{startDate, endDate, startTime, weekCnt};
    }
}
