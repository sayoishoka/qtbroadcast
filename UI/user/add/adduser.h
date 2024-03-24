#ifndef ADDUSER_H
#define ADDUSER_H
#include <QWidget>
#include <QSqlQuery>
#include <QMouseEvent>
#include <QMessageBox>
#include "UI/user/user_manage.h"
#include "function/dbinteraction/dbselectupdate.h"
namespace Ui {
class AddUser;
}
class User_Manage;
class AddUser : public QWidget
{
    Q_OBJECT

public:
    void user_modify_slot();
    void user_add_slot();
    void role_modify_slot();
    void role_add_slot();
    void get_user_data(QStringList* text);
    void get_role_data(QStringList* text);
    void getRolenNumber(QSqlQuery& query);
    static AddUser* getAddUser(){
        static AddUser instance;
        return &instance;
    };
    ~AddUser();
private:
    User_Manage *a;
    QMap<QString,int> usermap;
    void mousePressEvent(QMouseEvent* event);
    void mouseMoveEvent(QMouseEvent* event);
    QPoint mOffset;//鼠标与左上角的偏移量
private slots:
    void on_exit_clicked();

    void on_yes_clicked();

    void on_yes_2_clicked();

    void on_exit_2_clicked();

    void on_yes_3_clicked();

    void on_exit_3_clicked();

    void on_yes_4_clicked();

    void on_exit_4_clicked();

private:
    explicit AddUser(QWidget *parent = nullptr);
    Ui::AddUser *ui;
};

#endif // ADDUSER_H
