#include "adduser.h"
#include "ui_adduser.h"

AddUser::AddUser(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AddUser)
{
    ui->setupUi(this);
    QWidget *background = new QWidget(this);
    background->lower();
    background->setStyleSheet("background : rgb(233, 233, 233); border-radius : 10px");
    background->resize(this->size());
}

void AddUser::getdata(QStringList *text)
{
//    ui->dateTimeEdit->setCalendarPopup(true);
//    ui->dateTimeEdit->setDateTime(QDateTime::currentDateTime());
    ui->id->setValue(text->at(0).toInt());
    ui->name->setText(text->at(1));
    ui->account->setText(text->at(2));
    ui->pass->setText(text->at(3));
    ui->role->setCurrentText(usermap.key(text->at(5).toInt()));
    if (text->at(4).toInt()==1){
        ui->status->setCurrentText("正常");
    }else{
        ui->status->setCurrentText("冻结");
    }

//    QString date = text->at(5).split(" ").at(0);
//    QString time = text->at(5).split(" ").at(1);
    //qDebug() << date << time;
    //    ui->dateTimeEdit->setDateTime(QDateTime::fromString(text->at(5),"yyyy-MM-dd hh:mm:ss"));
}

void AddUser::getRolenNumber(QSqlQuery &query)
{
    usermap.clear();
    ui->role->clear();
    while(query.next()){
        usermap.insert(query.value(1).toString(),query.value(0).toInt());
        ui->role->addItem(query.value(1).toString());
    }
}

AddUser::~AddUser()
{
    delete ui;
}

void AddUser::mousePressEvent(QMouseEvent *event)
{
    mOffset = event->globalPos() - this->pos();
}

void AddUser::mouseMoveEvent(QMouseEvent *event)
{
    this->move(event->globalPos() - mOffset);
}

void AddUser::on_exit_clicked()
{
    this->close();
    ui->name->setText("");
    ui->account->setText("");
    ui->pass->setText("");
}

void AddUser::on_yes_clicked()
{
    QString id = QString::number(ui->id->value());
    QString name = ui->name->text();
    QString account = ui->account->text();
    QString pass = ui->pass->text();
    QString role = QString::number(usermap.value(ui->role->currentText()));
    QString status = "0";
    if (ui->status->currentIndex()==0){
        status = "1";
    }else{
        status = "0";
    }


        // BroadcastMain::exeSql("UPDATE USER SET user_nickname = '"+name+"',user_name = '"+account+"',user_pw = '"+pass+"',role_no = '"+role+"',user_status = '"+status+"' WHERE user_id = '"+id+"'");
        this->close();
//    }
}
