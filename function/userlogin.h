#ifndef USERLOGIN_H
#define USERLOGIN_H

#include <QObject>
#include<QJsonObject>
class userlogin : public QObject
{
    Q_OBJECT


private:
    QJsonObject login(QJsonObject);
    void registerMethods();

    QJsonObject getUserInfo(const QString &accountNumber, const QString &password);

    static userlogin * userlogin1;

    QJsonObject regist(QJsonObject jsonObj);
public:
    explicit userlogin(QObject *parent = nullptr);

    static userlogin* getlogin(){

        return userlogin1;
    }
};

#endif // USERLOGIN_H
