#include "databaseconfig.h"
#include "ui_databaseconfig.h"
//DatabaseConfig *DatabaseConfig::a = nullptr;
DatabaseConfig::DatabaseConfig(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DatabaseConfig)
{
    ui->setupUi(this);
    settings = new QSettings("D:/qtproject1/BroadcastServer/mysqlprofile.ini", QSettings::IniFormat);
    ip=settings->value( "/mysql/ip").toString();
    port=settings->value( "/mysql/port").toInt();
    database=settings->value( "/mysql/database").toString();
    username=settings->value( "/mysql/username").toString();
    userpassword=settings->value( "/mysql/userpassword").toString();
}

DatabaseConfig::~DatabaseConfig()
{
    delete ui;
}

void DatabaseConfig::on_yes_clicked()
{
    if(settings->contains(tr("/mysql/ip"))&&settings->contains(tr("/mysql/port"))&&settings->contains(tr("/mysql/database"))
            &&settings->contains(tr("/mysql/username"))&&settings->contains(tr("/mysql/userpassword")))
    {
        ip=ui->IP->text();
        port=ui->Port->text().toInt();
        database=ui->Database->text();
        username=ui->UserName->text();
        userpassword=ui->UserPassword->text();
        settings->beginGroup(tr("mysql"));
        settings->setValue("ip",ip);
        settings->setValue("port",port);
        settings->setValue("database",database);
        settings->setValue("username",username);
        settings->setValue("userpassword",userpassword);
        settings->endGroup();
    } else {
        ip=ui->IP->text();
        port=ui->Port->text().toInt();
        database=ui->Database->text();
        username=ui->UserName->text();
        userpassword=ui->UserPassword->text();
        settings->beginGroup(tr("mysql"));
        settings->setValue("ip",ip);
        settings->setValue("port",port);
        settings->setValue("database",database);
        settings->setValue("username",username);
        settings->setValue("userpassword",userpassword);
        settings->endGroup();
    }
    this->close();
    emit Showdbconfig();
}

void DatabaseConfig::on_no_clicked()
{
    this->close();
}
