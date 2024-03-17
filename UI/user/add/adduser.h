#ifndef ADDUSER_H
#define ADDUSER_H



#include <QWidget>
#include <QSqlQuery>
#include <QMouseEvent>
#include <QMessageBox>
#include "function/dbinteraction/dbselectupdate.h"
namespace Ui {
class AddUser;
}

class AddUser : public QWidget
{
    Q_OBJECT

public:
    void getdata(QStringList* text);
    void getRolenNumber(QSqlQuery& query);
    static AddUser* getAddUser(){
        static AddUser instance;
        return &instance;
    };
    ~AddUser();
private:
    QMap<QString,int> usermap;
    void mousePressEvent(QMouseEvent* event);
    void mouseMoveEvent(QMouseEvent* event);
    QPoint mOffset;//鼠标与左上角的偏移量
private slots:
    void on_exit_clicked();

    void on_yes_clicked();

private:
    explicit AddUser(QWidget *parent = nullptr);
    Ui::AddUser *ui;
};

#endif // ADDUSER_H
