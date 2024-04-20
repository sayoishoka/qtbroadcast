#ifndef RIGHTS_MANAGEMENT_H
#define RIGHTS_MANAGEMENT_H

#include <QWidget>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <connect/tcp_ip/tcplink.h>
#include <connect/dispatcher.h>
namespace Ui {
class Rights_management;
}

class Rights_management : public QWidget
{
    Q_OBJECT

public:
    static Rights_management* getRights_management(){
        static Rights_management instance;
        return &instance;
    };

    ~Rights_management();

private:
    QJsonObject getRoleFunc(QJsonObject &obj);
    QJsonObject addPms(QJsonObject &obj);
    QJsonObject removePms(QJsonObject &obj);
    explicit Rights_management(QWidget *parent = nullptr);
    Ui::Rights_management *ui;
};

#endif // RIGHTS_MANAGEMENT_H
