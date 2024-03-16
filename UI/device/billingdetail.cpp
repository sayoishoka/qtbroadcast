#include "billingdetail.h"
#include "billing_manager.h"
#include "UI/device/device_manager.h"
#include "qdebug.h"
#include "qsqlerror.h"
#include "qtimer.h"
#include "ui_billingdetail.h"
#include<UI/Mydata.h>

billingdetail::billingdetail(billing_Data billing, QSqlQuery query)
    : QWidget()
    , ui(new Ui::billingdetail)
{
    ui->setupUi(this);
    this->setWindowTitle("订单详细信息");
    showbilling(billing);
    showdev(query);
    billing_manager *dev = new billing_manager;
    connect(this,SIGNAL(stopbilling(int,QDateTime)),dev,SLOT(endbilling(int,QDateTime)));
    if(billing.ostatus==1){
        timer = new QTimer();
        connect(timer,&QTimer::timeout,this,&billingdetail::updateUsetime);
        timer->start(1000);

    }
    else
        ui->stopbtn->setHidden(true);



}

billingdetail::~billingdetail()
{
    delete ui;
}

void billingdetail::prepareongoing(){
    ui->tableWidget->setColumnCount(8);
    QStringList header;
    header<<"设备编号"<<"设备名称"<<"设备音量"<<"设备位置经度"<<"设备位置纬度"<<"每分钟价格"<<"每小时价格"<<"每天价格";

    ui->tableWidget->setHorizontalHeaderLabels(header);
    ui->tableWidget->horizontalHeader()->setStretchLastSection(true);
    ui->tableWidget->horizontalHeader()->setHighlightSections(false);
    ui->tableWidget->setStyleSheet("selection-background-color:#9ceaea");
    // ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableWidget->setSelectionMode(QTableWidget::SingleSelection);
}

void billingdetail::preparecomplain(){
    ui->tableWidget->setColumnCount(7);
    QStringList header;
    header<<"设备编号"<<"设备名称"<<"设备位置经度"<<"设备位置纬度"<<"每分钟价格"<<"每小时价格"<<"每天价格";

    ui->tableWidget->setHorizontalHeaderLabels(header);
    ui->tableWidget->horizontalHeader()->setStretchLastSection(true);
    ui->tableWidget->horizontalHeader()->setHighlightSections(false);
    ui->tableWidget->setStyleSheet("selection-background-color:#9ceaea");
    // ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableWidget->setSelectionMode(QTableWidget::SingleSelection);
}

void billingdetail::showbilling(billing_Data billing){

    int         oId     = billing.oId;
    int         userId  = billing.userId;
    int         cost    = billing.cost;
    QDateTime   beginTime = billing.begingTime;
    int         rentalTime = billing.rentalTime;
    int         useTime = billing.usetime;
    int         ostatus = billing.ostatus;
    QDateTime   endTime =  billing.endTime;
    QDateTime   currenttime =  QDateTime::currentDateTime();


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

    QString  endtime;
    if(useTime<rentalTime)
        endtime = "未结束";
    else
        endtime = endTime.toString("yyyy-MM-dd hh:mm:ss");




    ui->oIdLineEdit->setText(QString::number(oId));
    ui->useIdLineEdit->setText(QString::number(userId));
    ui->costLineEdit->setText(QString::number(cost));
    ui->beginTimeLineEdit->setText(begint);
    ui->rentTimeLineEdit->setText(rentaltime);
    ui->userTimeLineEdit->setText(usetime);
    ui->endTimeLineEdit->setText(endtime);

    if(ostatus==1){
        prepareongoing();

    }
    else{
        preparecomplain();
    }

}
void billingdetail::showdev(QSqlQuery query){

    int i=0;
    while(query.next()){
        ui->tableWidget->insertRow(i);
        int devNo = query.value(1).toInt();

        qDebug()<<query.value(1).toString();
        QSqlQuery query1("select * from device where devNo = ?");
        query1.addBindValue(devNo);
        query1.exec();

        query1.first();
        qDebug()<<query1.value(1).toString();

        QString devName   = query1.value(1).toString();
        int     devStatus = query1.value(2).toInt();
        int     volume    = query1.value(3).toInt();
        int     groupNo   = query1.value(4).toInt();
        double  longitude = query1.value(5).toDouble();
        double  latitude  = query1.value(6).toDouble();
        int     minute    = query1.value(8).toInt();
        int     hour    = query1.value(9).toInt();
        int     day    = query1.value(10).toInt();
        int j = 0;

        ui->tableWidget->setItem(i,j++,new QTableWidgetItem(QString::number(devNo)));
        ui->tableWidget->setItem(i,j++,new QTableWidgetItem(devName));


        if(ui->tableWidget->horizontalHeaderItem(2)->text()=="设备音量"){
            MySlider *slider = new MySlider;
            device_manager dev;
            dev.setSliderStyle(slider);
            slider->setRange(0, 100);  // 假设volume的范围是0到100
            slider->setValue(volume);
            QWidget w;

            slider->setProperty("devNo", devNo);  // 存储设备的 ID

            // 连接 sliderReleased 信号到 updateDatabase 槽
            // connect(slider,QSlider)

            connect(slider, &MySlider::sliderReleased, this, &billingdetail::updateVolume);
            ui->tableWidget->setCellWidget(i, j++, slider);
        }


        ui->tableWidget->setItem(i,j++,new QTableWidgetItem(QString::number(longitude, 'f', 8)));
        ui->tableWidget->setItem(i,j++,new QTableWidgetItem(QString::number(latitude, 'f', 8)));
        ui->tableWidget->setItem(i,j++,new QTableWidgetItem(QString::number(minute)));
        ui->tableWidget->setItem(i,j++,new QTableWidgetItem(QString::number(hour)));
        ui->tableWidget->setItem(i,j++,new QTableWidgetItem(QString::number(day)));

        i++;
    }

    ui->tableWidget->resizeColumnsToContents();
}

void billingdetail::updateVolume()
{
    MySlider *slider = qobject_cast<MySlider *>(sender());
    if (slider) {
        int value = slider->value();
        int devId = slider->property("devNo").toInt();  // 获取设备的 ID

        // 创建一个新的 QSqlQuery 对象
        QSqlQuery query;
        // 设置 SQL 语句
        query.prepare("UPDATE device SET volume = :volume WHERE devNo = :id");
        query.bindValue(":volume", value);
        query.bindValue(":id", devId);
        // 执行 SQL 语句
        if (!query.exec()) {
            qDebug() << "Failed to update database: " << query.lastError();
        }
    }
}

void billingdetail::updateUsetime(){
    QDateTime currenttime = QDateTime::currentDateTime();
    QDateTime beginTime = QDateTime::fromString(ui->beginTimeLineEdit->text(),"yyyy-MM-dd hh:mm:ss");
    int useTime = beginTime.msecsTo(currenttime) / 1000;

    if(useTime == ui->rentTimeLineEdit->text().toInt()){
        ui->endTimeLineEdit->setText(currenttime.toString("yyyy-MM-dd hh:mm:ss"));
        timer->stop();
    }
    else{
        QString     usetime = QString("%1:%2:%3")
                              .arg(useTime/3600, 1, 10, QChar('0'))
                              .arg(useTime%3600/60, 2, 10, QChar('0'))
                              .arg(useTime%60, 2, 10, QChar('0'));
        ui->userTimeLineEdit->setText(usetime);
    }

}


void billingdetail::on_stopbtn_clicked()
{
    int oId = ui->oIdLineEdit->text().toInt();
    QDateTime currenttime = QDateTime::currentDateTime();
    emit stopbilling(oId,currenttime);
    ui->endTimeLineEdit->setText(currenttime.toString("yyyy-MM-dd hh:mm:ss"));
    timer->stop();
    ui->stopbtn->setHidden(true);
}

