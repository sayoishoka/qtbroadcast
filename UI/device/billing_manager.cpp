#include "billing_manager.h"
#include "UI/Mydata.h"
#include "qdatetime.h"
#include "qsqlquery.h"
#include "qtimer.h"
#include "ui_billing_manager.h"

billing_manager::billing_manager(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::billing_manager)
{
    ui->setupUi(this);

    checkAndUpdateRecords();
    prepare();
    selectall();


    QTimer *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &billing_manager::updateTime);

    // 开始计时器，每秒触发一次 timeout() 信号
    timer->start(1000);
    connect(ui->completedOrderstableWidget,SIGNAL(itemDoubleClicked(QTableWidgetItem*)),this,SLOT(query_Detail_INfor(QTableWidgetItem*)));
    connect(ui->ongoingOrderstableWidget,SIGNAL(itemDoubleClicked(QTableWidgetItem*)),this,SLOT(query_Detail_INfor(QTableWidgetItem*)));

}

billing_manager::~billing_manager()
{
    delete ui;
}


void billing_manager::prepare(){

    QStringList header;
    header<<"订单编号"<<"用户编号"<<"价格"<<"开始时间"<<"租借时间"<<"使用时间";

    ui->ongoingOrderstableWidget->setColumnCount(6);
    ui->ongoingOrderstableWidget->setHorizontalHeaderLabels(header);
    ui->ongoingOrderstableWidget->horizontalHeader()->setHighlightSections(false);
    ui->ongoingOrderstableWidget->setStyleSheet("selection-background-color:#9ceaea");
    ui->ongoingOrderstableWidget->horizontalHeader()->setStretchLastSection(true);
    ui->ongoingOrderstableWidget->setSelectionBehavior(QTableWidget::SelectRows);
    ui->ongoingOrderstableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);


    header<<"结束时间";
    ui->completedOrderstableWidget->setColumnCount(7);
    ui->completedOrderstableWidget->setHorizontalHeaderLabels(header);
    ui->completedOrderstableWidget->horizontalHeader()->setHighlightSections(false);
    ui->completedOrderstableWidget->setStyleSheet("selection-background-color:#9ceaea");
    ui->completedOrderstableWidget->horizontalHeader()->setStretchLastSection(true);
    ui->completedOrderstableWidget->setSelectionBehavior(QTableWidget::SelectRows);
    ui->completedOrderstableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
}

void billing_manager ::selectall(){

    QSqlQuery query("select * from  billing");
    query.exec();
    showdata(query);


}

void billing_manager::showdata(QSqlQuery query){
    int h = ui->ongoingOrderstableWidget->rowCount();
    for(int row = 0;row < h;row++)
    {
        ui->ongoingOrderstableWidget->removeRow(0);
    }

    int h1 = ui->completedOrderstableWidget->rowCount();
    for(int row = 0;row < h1;row++)
    {
        ui->completedOrderstableWidget->removeRow(0);
    }

    int i=0,j=0;
    while(query.next()){

        int         oId     = query.value(0).toInt();
        int         userId  = query.value(1).toInt();
        int         cost    = query.value(2).toDouble();
        QDateTime   beginTime = query.value(3).toDateTime();
        int         rentalTime = query.value(4).toInt();
        int         useTime = query.value(5).toInt();;
        int         ostatus = query.value(6).toInt();
        QDateTime   currenttime =  QDateTime::currentDateTime();
        if(rentalTime==useTime && ostatus) endbilling(oId,currenttime);

        if(ostatus==1){
            ui->ongoingOrderstableWidget->insertRow(i);

            QString     begint = beginTime.toString("yyyy-MM-dd hh:mm:ss");


            QString     rentaltime = QString("%1:%2:%3")
                                   .arg(rentalTime/3600, 1, 10, QChar('0'))
                                   .arg(rentalTime%3600/60, 2, 10, QChar('0'))
                                   .arg(rentalTime%60, 2, 10, QChar('0'));



            useTime = beginTime.msecsTo(currenttime) / 1000;

            if(useTime>rentalTime)useTime = rentalTime;
            QString     usetime = QString("%1:%2:%3")
                                   .arg(useTime/3600, 1, 10, QChar('0'))
                                   .arg(useTime%3600/60, 2, 10, QChar('0'))
                                   .arg(useTime%60, 2, 10, QChar('0'));





            ui->ongoingOrderstableWidget->setItem(i,0,new QTableWidgetItem(QString::number(oId)));
            ui->ongoingOrderstableWidget->setItem(i,1,new QTableWidgetItem(QString::number(userId)));
            ui->ongoingOrderstableWidget->setItem(i,2,new QTableWidgetItem(QString::number(cost)));
            ui->ongoingOrderstableWidget->setItem(i,3,new QTableWidgetItem(begint));
            ui->ongoingOrderstableWidget->setItem(i,4,new QTableWidgetItem(rentaltime));
            ui->ongoingOrderstableWidget->setItem(i,5,new QTableWidgetItem(usetime));
            ui->ongoingOrderstableWidget->setItem(i,5,new QTableWidgetItem(usetime));


            i++;
        }
        else{

            ui->completedOrderstableWidget->insertRow(j);

            QString  begint = beginTime.toString("yyyy-MM-dd hh:mm:ss");


            QString rentaltime = QString("%1:%2:%3")
                                     .arg(rentalTime/3600, 1, 10, QChar('0'))
                                     .arg(rentalTime%3600/60, 2, 10, QChar('0'))
                                     .arg(rentalTime%60, 2, 10, QChar('0'));



            useTime = beginTime.msecsTo(currenttime) / 1000;
            if(useTime>rentalTime)useTime = rentalTime;
            QString usetime = QString("%1:%2:%3")
                                   .arg(useTime/3600, 1, 10, QChar('0'))
                                   .arg(useTime%3600/60, 2, 10, QChar('0'))
                                   .arg(useTime%60, 2, 10, QChar('0'));

            QDateTime endTime =query.value(7).toDateTime();;
            QString  endtime = endTime.toString("yyyy-MM-dd hh:mm:ss");



            ui->completedOrderstableWidget->setItem(j,0,new QTableWidgetItem(QString::number(oId)));
            ui->completedOrderstableWidget->setItem(j,1,new QTableWidgetItem(QString::number(userId)));
            ui->completedOrderstableWidget->setItem(j,2,new QTableWidgetItem(QString::number(cost)));
            ui->completedOrderstableWidget->setItem(j,3,new QTableWidgetItem(begint));
            ui->completedOrderstableWidget->setItem(j,4,new QTableWidgetItem(rentaltime));
            ui->completedOrderstableWidget->setItem(j,5,new QTableWidgetItem(usetime));
            ui->completedOrderstableWidget->setItem(j,6,new QTableWidgetItem(endtime));


            j++;
        }

    }
    ui->ongoingOrderstableWidget->resizeColumnsToContents();
    ui->completedOrderstableWidget->resizeColumnsToContents();
}

void billing_manager::updateTime()
{
    QDateTime currenttime = QDateTime::currentDateTime();

    // 创建一个 QSqlQuery 对象
    QSqlQuery query;

    // 准备 SQL 更新语句
    query.prepare("UPDATE billing SET useTime = TIMESTAMPDIFF(SECOND, beginTime, :currenttime) WHERE ostatus = 1");

    // 绑定参数
    query.bindValue(":currenttime", currenttime);

    // 执行 SQL 更新语句
    query.exec();

    // 重新查询并显示数据
    selectall();
}

void billing_manager::endbilling(int oId ,QDateTime endtime){



    QSqlQuery query;

    // 准备 SQL 更新语句
    query.prepare("UPDATE billing SET ostatus = 0 ,endTime = :endtime WHERE oId = :oId");

    // 绑定参数
    query.bindValue(":oId", oId);
    query.bindValue(":endtime", endtime);
    // 执行 SQL 更新语句
    query.exec();
}

void billing_manager::checkAndUpdateRecords() {
    // 创建一个 QSqlQuery 对象
    QSqlQuery query;

    // 准备 SQL 更新语句
    query.prepare("UPDATE billing SET ostatus = 0,usetime=rentalTime, endTime = DATE_ADD(beginTime, INTERVAL rentalTime SECOND) WHERE TIMESTAMPDIFF(SECOND, beginTime, NOW()) >= rentalTime AND ostatus = 1");

    // 执行 SQL 更新语句
    query.exec();
}

void billing_manager::query_Detail_INfor(QTableWidgetItem *item)
{
    QTableWidget* tableWidget = qobject_cast<QTableWidget*>(sender());
    int row = item->row(); // 获取被双击的行

    // 获取订单号
    int oId = tableWidget->item(row, 0)->text().toInt();

    // 在billing表中查询信息
    QSqlQuery query;
    query.prepare("SELECT * FROM billing WHERE oId = :oId");
    query.bindValue(":oId", oId);
    query.exec();


    query.next();

        billing_Data billing;
        billing.oId = query.value(0).toInt();
        billing.userId = query.value(1).toInt();
        billing.cost = query.value(2).toDouble();
        billing.begingTime = query.value(3).toDateTime();
        billing.rentalTime = query.value(4).toInt();
        billing.usetime = query.value(5).toInt();
        billing.ostatus = query.value(6).toInt();
        billing.endTime = query.value(7).toDateTime();

        // 在这里处理billing的信息


    // 在billing_device表中获取该订单所对应的多个设备
    query.prepare("SELECT * FROM billing_device WHERE oId = :oId");
    query.bindValue(":oId", oId);
    query.exec();



    billingdetail *detail = new billingdetail(billing,query);
    detail->show();


}
