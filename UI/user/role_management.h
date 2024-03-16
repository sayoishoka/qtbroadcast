#ifndef ROLE_MANAGEMENT_H
#define ROLE_MANAGEMENT_H

#include "broadcastmain.h"
#include <QWidget>
#include <QSqlQuery>
#include <QScrollBar>
namespace Ui {
class Role_management;
}

class Role_management : public QWidget
{
    Q_OBJECT

public:

    static Role_management* getRole_management(){
        static Role_management instance;
        return &instance;
    };
    ~Role_management();

private:
    QTimer* timer;
    void setTable();
    void update_data();//更新表格数据
    QJsonObject getAllRole(QJsonObject &obj);
    QJsonObject addRole(QJsonObject &obj);
    QJsonObject removeRole(QJsonObject &obj);
    explicit Role_management(QWidget *parent = nullptr);
    Ui::Role_management *ui;
};

#endif // ROLE_MANAGEMENT_H
