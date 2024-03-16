#include "broadcastmain.h"
#include "UI/user/user_manage.h"
#include "ui_broadcastmain.h"
#include "UI/databaseconfig.h"
#include"function/userlogin.h"
#include"function/device.h"
#include"function/group.h"
#include <QDebug>
#include <QSqlQuery>
#include <QMessageBox>
#include <QSqlError>
BroadcastMain::BroadcastMain(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::BroadcastMain)
{
    menu = new Menu();
    tcp = TcpLink::getTcpLink();
    databaseconfig = new DatabaseConfig();
    ui->setupUi(this);
    init();
    databaselink();
    userman = User_Manage::getUser_Manage();
    connect(databaseconfig,&DatabaseConfig::Showdbconfig,[=](){
        databaselink();
    });

    httpserver::getserver()->Listen(8080);
    userlogin::getlogin();
    GroupModule::getInstance();
    DeviceModule::getInstance();

//    connect(userman,&User_Manage::getdata,[=](){
//        QSqlQuery query;
//        query.exec("select * from user");
//        while(query.next()){
//            User user;
//            user.id=query.value(0).toInt();
//            user.name=query.value(1).toString();
//            user.accountnumber=query.value(2).toString();
//            user.password=query.value(3).toString();
//            user.role=query.value(4).toString();
//            user.status=query.value(5).toInt();
//            userdata<<user;
//        }
//    });
}

BroadcastMain::~BroadcastMain()
{
    delete ui;
}


///
/// \brief BroadcastMain::on_register_2_clicked
/// 注册按钮的响应函数
void BroadcastMain::on_register_2_clicked()
{
    qDebug() << "打开注册";
    ui->stackedWidget->setCurrentIndex(1);
}

void BroadcastMain::on_return_2_clicked()
{
    ui->stackedWidget->setCurrentIndex(0);
}
///
/// \brief BroadcastMain::on_exit_clicked
///退出按钮的响应函数
void BroadcastMain::on_exit_clicked()
{
    qApp->quit();
}
///
/// \brief BroadcastMain::on_logon_clicked
///登录按钮的响应函数
void BroadcastMain::on_logon_clicked()
{

    QString username=ui->user_name->text();
    QString userpsd=ui->user_pwd->text();
    QSqlQuery query;
    if(QString::localeAwareCompare(username,"")==0||QString::localeAwareCompare(userpsd,"")==0){
          QMessageBox::information(this, "提示", "用户名或密码为空");
    }else{
        query.exec("select * from user where user_name='" + username +"'");
        qDebug()<<query.size();
        if (query.size()==-1||query.size()==0){
            QMessageBox::information(this, "提示", "用户名或密码有误");
        }else{
            while(query.next()){
                qDebug()/*<<query.value("id").toInt()*/
                         <<query.value(0).toInt()
                        <<query.value(1).toString().toUtf8().data()
                       <<query.value(2).toString().toUtf8().data()
                      <<query.value(3).toString().toUtf8().data()
                     <<query.value(4).toString().toUtf8().data()
                    <<query.value(5).toInt();
                if (QString::localeAwareCompare(userpsd,query.value("user_pw").toString())!=0){
                    QMessageBox::information(this, "提示", "用户名或密码有误");
                }else{
                    this->hide();
                    Menu::UserName(query.value(1).toString());
                    menu->show();
                }
            }
        }
    }

//    QJsonObject obj;
//    obj.insert("request", "setDevsVolume");
//    obj.insert("volume", 70);
//    QJsonArray t;
//    t.append(1);
//    t.append(2);
//    obj.insert("devNo", t);
//    qDebug()<<obj;
//    QJsonObject ui = Device_Management::getDevice_Management()->setDevsVolume(obj);//测试收发请求
//    qDebug()<<ui;
}
///
/// \brief BroadcastMain::init
///登录界面的ui属性设置
void BroadcastMain::init()
{
    this->setWindowTitle("登录");
    ui->user_pwd->setPlaceholderText("输入用户密码");
    ui->user_name->setPlaceholderText("输入用户名");
}
///
/// \brief BroadcastMain::databaselink
///数据库的连接配置
void BroadcastMain::databaselink()
{
    QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL");
    QSettings *settings = new QSettings("D:/qtproject1/qtBroadcastServer/mysqlprofile.ini", QSettings::IniFormat);
//        db.setHostName("127.0.0.1");  //连接本地主机
//        db.setPort(3306);
//        db.setDatabaseName("qtserver");
//        db.setUserName("root");
//        db.setPassword("123456");
        if(settings->contains(tr("/mysql/ip"))&&settings->contains(tr("/mysql/port"))&&settings->contains(tr("/mysql/database"))
                &&settings->contains(tr("/mysql/username"))&&settings->contains(tr("/mysql/userpassword")))
        {
//            db.setHostName(settings->value( "/mysql/ip").toString());  //连接本地主机
//            db.setPort(settings->value( "/mysql/port").toInt());
//            db.setDatabaseName(settings->value( "/mysql/database").toString());
//            db.setUserName(settings->value( "/mysql/username").toString());
//            db.setPassword(settings->value( "/mysql/userpassword").toString());
        } else {
            settings->beginGroup(tr("mysql"));
            settings->setValue("ip","127.0.0.1");
            settings->setValue("port","3306");
            settings->setValue("database","qtserver");
            settings->setValue("username","root");
            settings->setValue("userpassword","123456");
            settings->endGroup();
//            db.setHostName(settings->value( "/mysql/ip").toString());  //连接本地主机
//            db.setPort(settings->value( "/mysql/port").toInt());
//            db.setDatabaseName(settings->value( "/mysql/database").toString());
//            db.setUserName(settings->value( "/mysql/username").toString());
//            db.setPassword(settings->value( "/mysql/userpassword").toString());
        }
        db.setHostName(settings->value( "/mysql/ip").toString());  //连接本地主机
        db.setPort(settings->value( "/mysql/port").toInt());
        db.setDatabaseName(settings->value( "/mysql/database").toString());
        db.setUserName(settings->value( "/mysql/username").toString());
        db.setPassword(settings->value( "/mysql/userpassword").toString());
        bool DataBaseLinkok = db.open();
        if (DataBaseLinkok){
            qDebug() << "数据库连接成功";
            this->show();
            //QMessageBox::information(this, "提示", "数据库连接成功");
        } else {
//            QMessageBox::information(this, "提示", "数据库连接失败");
            databaseconfig = DatabaseConfig::getDatabaseConfig();
            databaseconfig->show();
        }
}
///
/// \brief BroadcastMain::on_register_3_clicked
///注册账号
void BroadcastMain::on_register_3_clicked()
{
    QString username=ui->newuser_name->text();
    QString userpsd1=ui->newuser_pwd1->text();
    QString userpsd2=ui->newuser_pwd2->text();
    QSqlQuery query;
    if(QString::localeAwareCompare(username,"")==0||QString::localeAwareCompare(userpsd1,"")==0||QString::localeAwareCompare(userpsd2,"")==0){
          QMessageBox::information(this, "提示", "用户名或密码为空");
    }else{
        query.exec("select * from user where user_accountnumber='" + username +"'");
        if (query.size()==-1||query.size()==0){
            if (QString::localeAwareCompare(userpsd1,userpsd2)==0){
                query.exec("INSERT INTO user (user_name, user_accountnumber, user_password) VALUES ('" + username + "','" +username + "', '" + userpsd1 + "')");
                QMessageBox::information(this, "提示", "新用户注册成功");
                ui->stackedWidget->setCurrentIndex(0);
                ui->user_name->setText("");
                ui->user_pwd->setText("");
            }else{
                QMessageBox::information(this, "提示", "两次密码不一致");
            }
        }else{
            QMessageBox::information(this, "提示", "已存在该用户");
        }
    }
}
