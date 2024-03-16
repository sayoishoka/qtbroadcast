#include "addupdatedev.h"
#include "qdebug.h"
#include "qsqlerror.h"
#include "ui_addupdatedev.h"

addUpdateDev::addUpdateDev(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::addUpdateDev)
{
    ui->setupUi(this);

    prepare();

}

addUpdateDev::~addUpdateDev()
{
    delete ui;
}


void addUpdateDev::prepare(){
    QSqlQuery query;
    if (query.exec("SELECT devNo, devName FROM device")) {
        while (query.next()) {
            int devNo = query.value(0).toInt();
            QString devName = query.value(1).toString();
            devNos.push_back(devNo);
            devNames.push_back(devName);
        }
    } else {
        qDebug() << "Failed to execute query: " << query.lastError();
    }

    if (query.exec("SELECT * FROM devgroup")) {
        while (query.next()) {
            int groupNo = query.value(0).toInt();
            QString groupName = query.value(1).toString();
            groupMap[groupName] = groupNo;
            ui->groupCbox->addItem(groupName);
        }
    }
    else{
        qDebug() << "Failed to execute query: " << query.lastError();
    }

}

void addUpdateDev::on_NoSpin_valueChanged(int arg1)
{
    if(arg1!=0){
        int flag = 1;
        for(int i=0;i<devNos.size();i++){
            if(arg1==devNos[i]){
                flag = 0;
            }
        }
        if(flag){
            ui->NoTipEdit->setText("√ 设备编号符合条件");
            ui->NoTipEdit->setStyleSheet("color: green;background-color: rgba(255, 255, 255, 0);border:0px;");
        }
        else{
            ui->NoTipEdit->setText("× 设备编号重复");
            ui->NoTipEdit->setStyleSheet("color: red;background-color: rgba(255, 255, 255, 0);border:0px;");
        }
    }
    else{
        ui->NoTipEdit->clear();
    }
}


void addUpdateDev::on_nameLineEdit_textChanged(const QString &arg1)
{
    if(arg1!=NULL){
        int flag = 1;
        for(int i=0;i<devNames.size();i++){
            if(arg1==devNames[i]){
                flag = 0;
            }
        }
        if(flag){
            ui->NameTipEdit->setText("√ 设备名称符合条件");
            ui->NameTipEdit->setStyleSheet("color: green;background-color: rgba(255, 255, 255, 0);border:0px;");

        }
        else{
            ui->NameTipEdit->setText("× 设备名称重复");
            ui->NameTipEdit->setStyleSheet("color: red;background-color: rgba(255, 255, 255, 0);border:0px;");
        }
    }
    else{
        ui->NoTipEdit->clear();
    }
}


void addUpdateDev::on_pushButton_clicked()
{
    dev_Data devdata;
    devdata.devNo     = ui->NoSpin->value();
    devdata.devName   =ui->nameLineEdit->text();
    devdata.devStatus =1;
    devdata.volume    =0;
    devdata.groupNo   =groupMap[ui->groupCbox->currentText()];
    devdata.longitude =ui->longitudeLineEdit->text().toDouble();
    devdata.latitude  =ui->longitudeLineEdit->text().toDouble();

    emit addDevData(devdata);

    this->close();
}


void addUpdateDev::on_pushButton_2_clicked()
{
    ui->NoSpin->cleanText();
    ui->NameTipEdit->clear();
    ui->NoTipEdit->clear();
    ui->nameLineEdit->clear();
    ui->groupCbox->clear();
    ui->longitudeLineEdit->clear();
    ui->latitudeLineEdit->clear();
}

