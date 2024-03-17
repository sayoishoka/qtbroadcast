#include "devicerent_manager.h"
#include "qcheckbox.h"
#include "qdebug.h"
#include "qpushbutton.h"
#include "qsqlquery.h"
#include "ui_devicerent_manager.h"
#include "function/dbinteraction/db_device_rent.h"
devicerent_manager::devicerent_manager(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::devicerent_manager)
{
    ui->setupUi(this);
    db_device_rent::getdb_device_rent();
    prepare();
    selectall();
}

devicerent_manager::~devicerent_manager()
{
    delete ui;
}




void devicerent_manager::prepare(){

    ui->tableWidget->setColumnCount(7);
    QStringList header;
    header<<"设备编号"<<"设备名称"<<"租借状态"<<"每分钟价格"<<"每小时价格"<<"每天价格"<<"修改租用情况";

    ui->tableWidget->setHorizontalHeaderLabels(header);
    ui->tableWidget->horizontalHeader()->setHighlightSections(false);
    ui->tableWidget->setStyleSheet("selection-background-color:#9ceaea");
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableWidget->horizontalHeader()->setStretchLastSection(false);
    ui->tableWidget->setSelectionMode(QTableWidget::SingleSelection);

}

void devicerent_manager ::selectall(){
    QSqlQuery query("select devNo,devName,rentStatus,minute,hour,day from  device");
    query.exec();
    showdata(query);
}


void devicerent_manager::showdata(QSqlQuery query){
    int h = ui->tableWidget->rowCount();
    for(int row = 0;row < h;row++)
    {
        ui->tableWidget->removeRow(0);
    }
    int i=0;
    while(query.next()){
        ui->tableWidget->insertRow(i);
        int     devNo     = query.value(0).toInt();
        QString devName   = query.value(1).toString();
        int     rentStatus= query.value(2).toInt();
        int     minute    = query.value(3).toInt();
        int     hour      = query.value(4).toInt();
        int     day       = query.value(5).toInt();




        ui->tableWidget->setItem(i,0,new QTableWidgetItem(QString::number(devNo)));
        ui->tableWidget->setItem(i,1,new QTableWidgetItem(devName));
        ui->tableWidget->setItem(i,2,new QTableWidgetItem(QString::number(rentStatus)));
        ui->tableWidget->setItem(i,3,new QTableWidgetItem(QString::number(minute)));
        ui->tableWidget->setItem(i,4,new QTableWidgetItem(QString::number(hour)));
        ui->tableWidget->setItem(i,5,new QTableWidgetItem(QString::number(day)));

        QPushButton *pushbutton = new QPushButton;
        if(rentStatus==0){
            pushbutton->setText("不能租借");
            pushbutton->setStyleSheet("color:red;");
            connect(pushbutton, &QPushButton::clicked, this, [=](){updaterentstatus(rentStatus,devNo);});


        }
        else if(rentStatus==1){
            pushbutton->setText("不可修改");
            pushbutton->setEnabled(false);
        }
        else{
            pushbutton->setText("可以租借");
            pushbutton->setStyleSheet("color:green;");
            connect(pushbutton, &QPushButton::clicked, this, [=](){updaterentstatus(rentStatus,devNo);});
        }

        ui->tableWidget->setCellWidget(i,6,pushbutton);

        i++;
    }
}

void devicerent_manager::updaterentstatus(int i,int devNo){
    QSqlQuery query;
    if(i){
        i=0;
    }
    else
        i=2;

    query.prepare(QString("update device set rentStatus = %1 where devNo = :devNo").arg(i));
    query.bindValue(":devNo",devNo);
    query.exec();
    selectall();
    qDebug()<<"3";
}
