#include "user_manage.h"
#include "add/adduser.h"
#include "ui_user_manage.h"
#include <QTimer>

User_Manage::User_Manage(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::User_Manage)
{
    ui->setupUi(this);
    au = AddUser::getAddUser();
    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, [=](){
        update_data();
    });//连接信号槽
    timer->start(3000);//3s更新一次
    update_data();
    setTable();
    Dispatcher::getDispatcher()->Register("getUserList",std::bind(&User_Manage::getUserList, this,std::placeholders::_1));
    Dispatcher::getDispatcher()->Register("addUser",std::bind(&User_Manage::addUser, this,std::placeholders::_1));
    Dispatcher::getDispatcher()->Register("setUserStatus",std::bind(&User_Manage::setUserStatus, this,std::placeholders::_1));
    Dispatcher::getDispatcher()->Register("resetUserPw",std::bind(&User_Manage::resetUserPw, this,std::placeholders::_1));
}

User_Manage::~User_Manage()
{
    delete ui;
}

void User_Manage::setTable()
{
//    ui->tableWidget->setColumnCount(6);//设置列
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(0,QHeaderView::ResizeToContents);
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(1,QHeaderView::ResizeToContents);
//    ui->tableWidget->horizontalHeader()->setSectionResizeMode(2,QHeaderView::ResizeToContents);
//    ui->tableWidget->horizontalHeader()->setSectionResizeMode(3,QHeaderView::ResizeToContents);
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(4,QHeaderView::ResizeToContents);
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(5,QHeaderView::ResizeToContents);
//    ui->tableWidget->horizontalHeader()->setStretchLastSection(true);
    ui->tableWidget->verticalHeader()->setVisible(false); //设置垂直头不可见,即左边序号不见
    ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers); //设置表格内容不可编辑
    ui->tableWidget->verticalScrollBar()->setStyleSheet("QScrollBar{background:transparent; width: 12px;}"
             "QScrollBar::handle{background:lightgray; border:2px solid transparent; border-radius:5px;}"
             "QScrollBar::handle:hover{background:gray;}"
             "QScrollBar::sub-line{background:transparent;}"
             "QScrollBar::add-line{background:transparent;}");
    ui->tableWidget->setStyleSheet("selection-background-color:rgb(34, 170, 75);"); //设置选中行的背景色
    // 设置选中行
        int rowToSelect = 0; // 要选中的行的索引
        QTableWidgetItem* selectedItem = ui->tableWidget->item(rowToSelect, 0);
        ui->tableWidget->setCurrentItem(selectedItem);

    dbSelectUpdate e;
    QSqlQuery query = e.getData_Sheet("select role_no,role_name from role");
    int number = query.size();
    au->getRolenNumber(query);
}

void User_Manage::update_data()
{
    dbSelectUpdate e;
    QSqlQuery query = e.getData_Sheet("select * from user");
    int number = query.size();
    int row = 0;
    if (number!=-1||number!=0){
        ui->tableWidget->setRowCount(number);//设置行
//        number=0;
        while(query.next()){
            for (int i=0;i<9;i++){
                QTableWidgetItem* item1 = new QTableWidgetItem(query.value(i).toString());
                ui->tableWidget->setItem(row,i,item1);
                if (i==7||i==8){
                    QTableWidgetItem* item1 = new QTableWidgetItem(query.value(i).toString().replace("T"," "));
                    ui->tableWidget->setItem(row,i,item1);
                }
            }
//            QTableWidgetItem* item1 = new QTableWidgetItem(query.value(0).toString());
//            QTableWidgetItem* item2 = new QTableWidgetItem(query.value(1).toString());
//            QTableWidgetItem* item3 = new QTableWidgetItem(query.value(2).toString());
//            QTableWidgetItem* item4 = new QTableWidgetItem(query.value(3).toString());
//            QTableWidgetItem* item5 = new QTableWidgetItem(query.value(4).toString());
//            QTableWidgetItem* item6 = new QTableWidgetItem(query.value(5).toString());
//            ui->tableWidget->setItem(number,0, item1);
//            ui->tableWidget->setItem(number,1, item2);
//            ui->tableWidget->setItem(number,2, item3);
//            ui->tableWidget->setItem(number,3, item4);
//            ui->tableWidget->setItem(number,4, item5);
//            ui->tableWidget->setItem(number,5, item6);
            row++;
        }
    }
}

QJsonObject User_Manage::getUserList(QJsonObject &obj)
{
    QString userName = obj.value("userName").toString();
    QString roleNo = QString::number(obj.value("roleNo").toInt());
    QJsonObject requestjson;
    QJsonArray userList;
    QJsonObject user;
    dbSelectUpdate e;
    QSqlQuery query = e.getData_Sheet("SELECT user.user_id,user.user_name,user.user_status,user.role_no,role.role_name FROM USER "
                                                   "LEFT JOIN role ON user.role_no=role.role_no "
                                                   "WHERE user_name LIKE '%"+userName+"%'");
    while(query.next()){
        user = QJsonObject();
        user.insert("userName",query.value(1).toString());
        if (query.value(2).toInt()==1){
            user.insert("userStatus",true);
        }else{
            user.insert("userStatus",false);
        }
        user.insert("roleNo",query.value(3).toString());
        user.insert("roleName",query.value(4).toString());
        userList.append(user);
    }
    requestjson.insert("response","reSelectUser");
    requestjson.insert("userList",userList);
    return requestjson;
}

QJsonObject User_Manage::addUser(QJsonObject &obj)
{
    QString userName = obj.value("userName").toString();
    QString roleNo = QString::number(obj.value("roleNo").toInt());
    QJsonObject requestjson;
    dbSelectUpdate e;
    QSqlQuery query = e.getData_Sheet("INSERT INTO USER (user_name,role_no) VALUES ('"+userName+"','"+roleNo+"')");
    requestjson.insert("response","reAddUser");
    requestjson.insert("status",true);
    return requestjson;
}

QJsonObject User_Manage::setUserStatus(QJsonObject &obj)
{
    QString userName = obj.value("userName").toString();
    QString userStatus;
    if (obj.value("userStatus").toBool()){
        userStatus = "1";
    }else{
        userStatus = "0";
    }
    dbSelectUpdate e;
    QSqlQuery query = e.getData_Sheet("UPDATE USER SET user_status = '"+userStatus+"' WHERE user_name = '"+userName+"'");
    QJsonObject requestjson;
    requestjson.insert("response","reSetUserStatus");
    requestjson.insert("status",true);
    return requestjson;
}

QJsonObject User_Manage::resetUserPw(QJsonObject &obj)
{
    QString userName = obj.value("userName").toString();
    dbSelectUpdate e;
    QSqlQuery query = e.getData_Sheet("UPDATE USER SET user_pw = 'jit1234' WHERE user_name = '"+userName+"'");
    QJsonObject requestjson;
    requestjson.insert("response","reResetUserPw");
    requestjson.insert("status",true);
    return requestjson;
}

void User_Manage::on_modify_clicked()
{

    QList<QTableWidgetItem*> items = ui->tableWidget->selectedItems();
    int count = items.count();
    QStringList text;
    for(int i = 0; i < count; i++)
   {
       int row = ui->tableWidget->row(items.at(i));
       QTableWidgetItem *item = items.at(i);
       text.append(item->text()); //获取内容
       qDebug() << item->text();
   }
   au->getdata(&text);
   au->show();
}

void User_Manage::on_delete_2_clicked()
{
    QMessageBox::StandardButton result=QMessageBox::question(this, "提示","是否删除所选中行数据");
    if (result==QMessageBox::Yes){
        qDebug() << "确定删除";
        QList<QTableWidgetItem*> items = ui->tableWidget->selectedItems();
        int count = items.count();
        QStringList text;
        for(int i = 0; i < count; i++)
        {
            int row = ui->tableWidget->row(items.at(i));
            QTableWidgetItem *item = items.at(i);
            text.append(item->text()); //获取内容
        }
        dbSelectUpdate e;
        e.exeSql("DELETE FROM user WHERE user_id="+text.at(0));
    }
}
