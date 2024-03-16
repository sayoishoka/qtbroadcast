#include "userlogin.h"
#include "connect/dispatcher.h"
#include "connect/dispatcher.h"
#include "qjsonobject.h"
#include "qsqlquery.h"
#include "userlogin.h"
userlogin* userlogin::userlogin1 = new userlogin;

userlogin::userlogin(QObject *parent)
    : QObject{parent}
{
    registerMethods();


}

void userlogin::registerMethods(){
    Dispatcher *dispatcher = Dispatcher::getDispatcher();

    dispatcher->Register("/login",std::bind(&userlogin::login, this,std::placeholders::_1));
    dispatcher->Register("regist",std::bind(&userlogin::regist, this,std::placeholders::_1));

}
QJsonObject userlogin::login(QJsonObject jsonObj){

    // 从 jsonObj 中提取用户名和密码
    QString username = jsonObj["username"].toString();
    QString password = jsonObj["password"].toString();

    qDebug()<<username;

    // 假设你有一个名为 `getUserInfo` 的函数，它接受一个用户名和密码，
    // 并返回一个包含用户信息的 QJsonObject
    QJsonObject userInfo = getUserInfo(username,password);

    // 检查用户名和密码是否匹配
    if (!userInfo.isEmpty()) {
        // 如果匹配，返回用户信息
        qDebug()<<"登陆成功";
        return QJsonObject{
         {"code", 200},
        {"message", "登陆成功"},
            {"data", userInfo}
        };
    } else {
        // 如果不匹配，返回错误信息
        return QJsonObject{
            {"code", 401},
            {"message", "用户名或密码错误"}
        };
    }

}

QJsonObject userlogin::getUserInfo(const QString& accountNumber, const QString& password) {

    QSqlQuery query;
    query.prepare("SELECT * FROM User WHERE user_accountnumber = :accountNumber ");
    query.bindValue(":accountNumber", accountNumber);
    query.exec();

    if (query.next()) {
         QString dbPassword = query.value("user_password").toString();
         if (dbPassword == password) {
        QJsonObject userInfo;
        userInfo["id"] = query.value("userId").toInt();
        userInfo["username"] = query.value("user_accountnumber").toString();
        userInfo["password"] = query.value("user_password").toString();
        return userInfo;
         }
         else{
             qDebug() << "密码错误";
             return QJsonObject();
         }
    } else {
        qDebug() << "用户名或密码错误";
        return QJsonObject();
    }
}
QJsonObject userlogin::regist(QJsonObject jsonObj){

}
