#include "../dbinteraction.h"

#include <QDebug>
#include <QJsonArray>
#include <QJsonObject>
#include <QSqlError>
#include <QSqlQuery>

std::tuple<bool, int>
SelectUser(QString userName, QString userPw)
{
    QSqlQuery query(QSqlDatabase::database(QString::number(reinterpret_cast<quintptr>(QThread::currentThreadId()))));
    query.prepare("select * from `user` "
                  "where `user_name` = :userName and `user_pw` = :userWd limit 1");
    query.bindValue(":userName", userName);
    query.bindValue(":userWd", userPw);

    if(!query.exec() || !query.next()) {
        qDebug() << "exec SelectUser failed!!!";
        qDebug() << query.lastError();
        return {};
    }
    else {
        bool status = query.value("user_status").toBool();
        int role    = query.value("role_no").toInt();
        return std::tuple<bool, int>(status, role);
    }
}

QVector<std::tuple<QString, int>> SelectPms(int roleNo)
{
    QSqlQuery query(QSqlDatabase::database(QString::number(reinterpret_cast<quintptr>(QThread::currentThreadId()))));

    query.prepare("select * from `permission` natural join `function` "
                  "where `fa_no` is null and `role_no` = :roleNo;");
    query.bindValue(":roleNo", roleNo);

    if(!query.exec()) {
        qDebug() << "exec SelectPms(int) failed!!!";
        qDebug() << query.lastError();
        return QVector<std::tuple<QString, int>>();
    }
    else {
        QVector<std::tuple<QString, int>> funcList;
        while(query.next()) {
            int funcNo       = query.value("func_no").toInt();
            QString funcName = query.value("func_name").toString();
            funcList.push_back(std::tuple<QString, int>(funcName, funcNo));
        }
        return funcList;
    }
}

QVector<std::tuple<QString, int>> SelectPms(int roleNo, int faFuncNo)
{
    QSqlQuery query(QSqlDatabase::database(QString::number(reinterpret_cast<quintptr>(QThread::currentThreadId()))));
    query.prepare("select * from `permission` natural join `function` "
                  "where `role_no` = :roleNo and `fa_no` = :fa_no;");
    query.bindValue(":roleNo", roleNo);
    query.bindValue(":fa_no", faFuncNo);

    QVector<std::tuple<QString, int>> funcList;
    if(!query.exec()) {
        qDebug() << "exec SelectPms(int, int) failed!!!";
        qDebug() << query.lastError();
        return funcList;
    }
    else {
        while(query.next()) {
            int funcNo       = query.value("func_no").toInt();
            QString funcName = query.value("func_name").toString();
            funcList.push_back(std::tuple<QString, int>(funcName, funcNo));
        }
        return funcList;
    }
}

QVector<std::tuple<QString, int>> SelectFunc()
{
    QSqlQuery query(QSqlDatabase::database(QString::number(reinterpret_cast<quintptr>(QThread::currentThreadId()))));
    QVector<std::tuple<QString, int>> funcList;
    if(!query.exec("select * from `function` where `fa_no` is null;")) {
        qDebug() << "exec SelectFunc() failed!!!";
        qDebug() << query.lastError();
        return funcList;
    }
    else {
        while(query.next()) {
            QString funcName = query.value("func_name").toString();
            int funcNo       = query.value("func_no").toInt();
            funcList.push_back(std::tuple<QString, int>(funcName, funcNo));
        }
        return funcList;
    }
}

QVector<std::tuple<QString, int>> SelectFunc(int faNo)
{
    QSqlQuery query(QSqlDatabase::database(QString::number(reinterpret_cast<quintptr>(QThread::currentThreadId()))));
    query.prepare("select * from `function` where `fa_no` = :faNo;");
    query.bindValue(":faNo", faNo);

    QVector<std::tuple<QString, int>> funcList;
    if(!query.exec()) {
        qDebug() << "exec SelectFunc(int) failed!!!";
        qDebug() << query.lastError();
        return funcList;
    }
    else {
        while(query.next()) {
            QString funcName = query.value("func_name").toString();
            int funcNo       = query.value("func_no").toInt();
            funcList.push_back(std::tuple<QString, int>(funcName, funcNo));
        }
        return funcList;
    }
}

bool AddPmsToRole(int roleNo, int funcNo)
{
    QSqlQuery query(QSqlDatabase::database(QString::number(reinterpret_cast<quintptr>(QThread::currentThreadId()))));
    query.prepare("insert into `permission`(`role_no`, `func_no`) values(:roleNo, :funcNo)");
    query.bindValue(":roleNo", roleNo);
    query.bindValue(":funcNo", funcNo);

    if(!query.exec()) {
        qDebug() << "exec AddPmsToRole failed!!!";
        qDebug() << query.lastError();
        return false;
    }
    else {
        return true;
    }
}

bool RemovePmsToRole(int roleNo, int funcNo)
{
    QSqlQuery query(QSqlDatabase::database(QString::number(reinterpret_cast<quintptr>(QThread::currentThreadId()))));
    query.prepare("delete from `permission` where `role_no` = :roleNo and `func_no` = :funcNo;");
    query.bindValue(":roleNo", roleNo);
    query.bindValue(":funcNo", funcNo);

    if(!query.exec()) {
        qDebug() << "exec ReMovePmsToRole failed!!!";
        qDebug() << query.lastError();
        return false;
    }
    else {
        if(query.numRowsAffected() < 1)
            return false;
        return true;
    }
}

bool SelectHasPms(int roleNo, int funcNo)
{
    QSqlQuery query(QSqlDatabase::database(QString::number(reinterpret_cast<quintptr>(QThread::currentThreadId()))));
    query.prepare("select * from `permission` "
                  "where `role_no` = :roleNo and `func_no` = :funcNo;");
    query.bindValue(":roleNo", roleNo);
    query.bindValue(":funcNo", funcNo);

    if(!query.exec()) {
        qDebug() << "exec SelectHasPms failed!!!";
        qDebug() << query.lastError();
        return false;
    }
    else {
        if(query.size() < 1)
            return false;
        return true;
    }
}

bool AddRole(QString roleName, int roleNo)
{
    QSqlQuery query(QSqlDatabase::database(QString::number(reinterpret_cast<quintptr>(QThread::currentThreadId()))));
    query.prepare("insert into `role`(`role_name`, `role_no`) values(:roleName, :roleNo)");
    query.bindValue(":roleName", roleName);
    query.bindValue(":roleNo", roleNo);

    if(!query.exec()) {
        qDebug() << "exec AddRole failed!!!";
        qDebug() << query.lastError();
        return false;
    }
    else {
        if(query.numRowsAffected() < 1)
            return false;
        return true;
    }
}

QVector<std::tuple<QString, int>> SelectAllRole()
{
    QVector<std::tuple<QString, int>> roleList;

    QSqlQuery query(QSqlDatabase::database(QString::number(reinterpret_cast<quintptr>(QThread::currentThreadId()))));
    if(!query.exec("select * from `role`;")) {
        qDebug() << "exec SelectAllRole failed!!!";
        qDebug() << query.lastError();
        return roleList;
    }
    else {
        while(query.next()) {
            QString roleName = query.value("role_name").toString();
            int roleNo       = query.value("role_no").toInt();
            roleList.push_back(std::tuple<QString, int>(roleName, roleNo));
        }
        return roleList;
    }
}

QVector<std::tuple<QString, int>> selectRoleByName(const QString &roleName)
{
    QVector<std::tuple<QString, int>> roleList;

    QSqlQuery query(QSqlDatabase::database(QString::number(reinterpret_cast<quintptr>(QThread::currentThreadId()))));

    QString sql("select * from `role` where `role_name` like '%" + roleName + "%';");
    if(!query.exec(sql)) {
        qDebug() << "exec selectRoleByName failed!!!";
        qDebug() << query.lastError();
        return roleList;
    }
    else {
        while(query.next()) {
            QString roleName = query.value("role_name").toString();
            int roleNo       = query.value("role_no").toInt();
            roleList.push_back(std::tuple<QString, int>(roleName, roleNo));
        }
        return roleList;
    }
}

bool RemoveRole(int roleNo)
{
    QSqlQuery query(QSqlDatabase::database(QString::number(reinterpret_cast<quintptr>(QThread::currentThreadId()))));
    query.prepare("delete from `role` where `role_no` = :roleNo;");
    query.bindValue(":roleNo", roleNo);

    if(!query.exec()) {
        qDebug() << "exec RemoveRole failed!!!";
        qDebug() << query.lastError();
        return false;
    }
    else {
        if(query.numRowsAffected() < 1)
            return false;
        return true;
    }
}

bool AddUser(QString userName, int roleNo)
{
    QSqlQuery query(QSqlDatabase::database(QString::number(reinterpret_cast<quintptr>(QThread::currentThreadId()))));
    query.prepare("insert into `user`(`user_name`, `user_pw`, `user_status`, `role_no`) "
                  "values(:userName, 'jit1234', 1, :roleNo)");
    query.bindValue(":userName", userName);
    query.bindValue(":roleNo", roleNo);

    if(!query.exec()) {
        qDebug() << "exec AddUser failed!!!";
        qDebug() << query.lastError();
        return false;
    }
    else {
        if(query.numRowsAffected() < 1)
            return false;
        return true;
    }
}

bool ModifyUserRole(QString userName, int newRoleNo)
{
    QSqlQuery query(QSqlDatabase::database(QString::number(reinterpret_cast<quintptr>(QThread::currentThreadId()))));
    query.prepare("update `user` set `role_no` = :roleNo  "
                  "where `user_name` = :userName");
    query.bindValue(":userName", userName);
    query.bindValue(":roleNo", newRoleNo);

    if(!query.exec()) {
        qDebug() << "exec ModifyUserRole failed!!!";
        qDebug() << query.lastQuery();
        qDebug() << query.lastError();
        return false;
    }
    else {
        if(query.numRowsAffected() < 1)
            return false;
        return true;
    }
}

QVector<User> SelectUserList(QString userName, int roleNo)
{
    QSqlQuery query(QSqlDatabase::database(QString::number(reinterpret_cast<quintptr>(QThread::currentThreadId()))));

    QString sql = "select * from `user` natural join `role` ";
    sql += QString("where `user_name` like '%" + userName + "%' ");
    if(roleNo != 0) {
        sql += QString("and `role_no` = %1;").arg(roleNo);
    }

    QVector<User> userList;
    if(!query.exec(sql)) {
        qDebug() << "exec SelectUserList failed!!!";
        qDebug() << query.lastError();
        return userList;
    }
    else {
        while(query.next()) {
            User user;
            user.userName   = query.value("user_name").toString();
            user.userPw     = query.value("user_pw").toString();
            user.roleName   = query.value("role_name").toString();
            user.roleNo     = query.value("role_no").toInt();
            user.userStatus = query.value("user_status").toBool();

            userList.push_back(user);
        }

        return userList;
    }
}

bool SetUserStatus(QString userName, bool userStatus)
{
    QSqlQuery query(QSqlDatabase::database(QString::number(reinterpret_cast<quintptr>(QThread::currentThreadId()))));
    query.prepare("update `user` set `user_status` = :userStatus "
                  "where `user_name` = :userName");
    query.bindValue(":userStatus", userStatus);
    query.bindValue(":userName", userName);

    if(!query.exec()) {
        qDebug() << "exec SetUserStatus failed!!!";
        qDebug() << query.lastError();
        return false;
    }
    else {
        if(query.numRowsAffected() < 1)
            return false;
        return true;
    }
}

bool ResetUsetPw(QString userName)
{
    QSqlQuery query(QSqlDatabase::database(QString::number(reinterpret_cast<quintptr>(QThread::currentThreadId()))));
    query.prepare("update `user` set `user_pw` = 'jit1234' "
                  "where `user_name` = :userName");
    query.bindValue(":userName", userName);

    if(!query.exec()) {
        qDebug() << "exec ResetUsetPw failed!!!";
        qDebug() << query.lastError();
        return false;
    }
    else {
        if(query.numRowsAffected() < 1)
            return false;
        return true;
    }
}
