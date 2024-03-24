#include "adduser.h"
#include "ui_adduser.h"

AddUser::AddUser(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::AddUser)
{
    ui->setupUi(this);
//    a = User_Manage::getUser_Manage();
//    connect(a,&User_Manage::user_modify,this,&AddUser::user_modify_slot);
//    connect(a,&User_Manage::user_add,this,&AddUser::user_add_slot);
    QWidget *background = new QWidget(this);
    background->lower();
    background->setStyleSheet("background : rgb(233, 233, 233); border-radius : 10px");
    background->resize(this->size());
}

void AddUser::get_user_data(QStringList *text)
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

void AddUser::get_role_data(QStringList *text)
{
    ui->id_3->setValue(text->at(0).toInt());
    ui->name_3->setText(text->at(1));
}

void AddUser::getRolenNumber(QSqlQuery &query)
{
    usermap.clear();
    ui->role->clear();
    while(query.next()){
        usermap.insert(query.value(1).toString(),query.value(0).toInt());
        ui->role->addItem(query.value(1).toString());
        ui->role_2->addItem(query.value(1).toString());
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
        dbSelectUpdate e;
        QSqlQuery namequery;
        QSqlQuery accquery;
        namequery = e.getData_Sheet("SELECT * FROM user WHERE user_name='"+name+"'");
        accquery = e.getData_Sheet("SELECT * FROM user WHERE user_accountnumber='"+account+"'");
        if (namequery.first()||accquery.first()) {
            QMessageBox::information(this, "提示", "检测到有重复数据无法修改");
        } else {
        e.exeSql("UPDATE USER SET user_nickname = '"+name+"',user_name = '"+account+"',user_pw = '"+pass+"',role_no = '"+role+"',user_status = '"+status+"' WHERE user_id = '"+id+"'");
        this->close();
        ui->name->setText("");
        ui->account->setText("");
        ui->pass->setText("");
    }
}

void AddUser::on_yes_2_clicked()
{
    QString id = QString::number(ui->id_2->value());
    QString name = ui->name_2->text();
    QString account = ui->account_2->text();
    QString pass = ui->pass_2->text();
    QString role = QString::number(usermap.value(ui->role_2->currentText()));
    QString status = "0";
    if (ui->status_2->currentIndex()==0){
        status = "1";
    }else{
        status = "0";
    }
        dbSelectUpdate e;
        QSqlQuery idquery;
        QSqlQuery namequery;
        QSqlQuery accquery;
        idquery = e.getData_Sheet("SELECT * FROM user WHERE user_id='"+ id +"'");
        namequery = e.getData_Sheet("SELECT * FROM user WHERE user_name='"+ name +"'");
        accquery = e.getData_Sheet("SELECT * FROM user WHERE user_accountnumber='"+ account +"'");
        if (idquery.first()||namequery.first()||accquery.first()) {
            QMessageBox::information(this, "提示", "检测到有重复数据无法添加");
        } else {
        e.exeSql("INSERT INTO user (user_id, user_nickname, user_name, user_pw, user_status, role_no) VALUES ('"+id+"', '"+name+"', '"+account+"', '"+pass+"', '"+status+"', '"+role+"')");
        this->close();
        ui->name_2->setText("");
        ui->account_2->setText("");
        ui->pass_2->setText("");
        }
}

void AddUser::on_exit_2_clicked()
{
    this->close();
    ui->name_2->setText("");
    ui->account_2->setText("");
    ui->pass_2->setText("");
}

void AddUser::user_modify_slot()
{
    ui->stackedWidget->setCurrentIndex(0);
}

void AddUser::user_add_slot()
{
    ui->stackedWidget->setCurrentIndex(1);
}

void AddUser::role_modify_slot()
{
    ui->stackedWidget->setCurrentIndex(2);
}

void AddUser::role_add_slot()
{
    ui->stackedWidget->setCurrentIndex(3);
}

void AddUser::on_yes_3_clicked()
{
    QString id = QString::number(ui->id_3->value());
    QString name = ui->name_3->text();
    dbSelectUpdate e;
    QSqlQuery namequery;
    namequery = e.getData_Sheet("SELECT * FROM role WHERE role_name='"+ name +"'");
    if (namequery.first()) {
        QMessageBox::information(this, "提示", "检测到有重复数据无法修改");
    } else {
    e.exeSql("UPDATE role SET role_name = '"+name+"' WHERE role_no = '"+id+"'");
    this->close();
    ui->name_3->setText("");
    }
}

void AddUser::on_exit_3_clicked()
{
    this->close();
    ui->name_3->setText("");
}

void AddUser::on_yes_4_clicked()
{
    QString id = QString::number(ui->id_4->value());
    QString name = ui->name_4->text();
    dbSelectUpdate e;
    QSqlQuery idquery;
    QSqlQuery namequery;
    idquery = e.getData_Sheet("SELECT * FROM role WHERE role_no='"+ id +"'");
    namequery = e.getData_Sheet("SELECT * FROM role WHERE role_name='"+ name +"'");
    if (idquery.first()||namequery.first()) {
        QMessageBox::information(this, "提示", "检测到有重复数据无法添加");
    } else {
    e.exeSql("INSERT INTO role (role_no, role_name) VALUES ('"+id+"', '"+name+"')");
    this->close();
    ui->name_4->setText("");
    }
}

void AddUser::on_exit_4_clicked()
{
    this->close();
    ui->name_4->setText("");
}
