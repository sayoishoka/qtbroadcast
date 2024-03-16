#include "device_manager.h"
#include "qmessagebox.h"
#include "qsqlerror.h"
#include "ui_device_manager.h"
#include<QSqlQuery>
#include<QDebug>

device_manager::device_manager(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::device_manager)
{
    ui->setupUi(this);
     // Connectdatabase();
    setcontrol();
    selectall();
    // ui->tableWidget->setCellWidget()
    connect(ui->tableWidget, &QTableWidget::itemChanged, this, &device_manager::onItemChanged);

}

device_manager::~device_manager()
{
    delete ui;
}


void device_manager::Connectdatabase()
{
    QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL");
    db.setHostName("localhost");
    db.setPort(3307);
    db.setDatabaseName("broadcast");//数据库名字
    db.setUserName("root");//用户姓名
    db.setPassword("123456");//用户密码
    if (!db.open()) {

        qDebug() << "无法连接到数据库";
        return;
    }
    else{
        qDebug() << "se";
    }

}


void device_manager::setcontrol()
{
    ui->tableWidget->setColumnCount(7);
    QStringList header;
    header<<"设备编号"<<"设备名称"<<"设备状态"<<"设备音量"<<"分组编号"<<"设备位置经度"<<"设备位置经度";

    ui->tableWidget->setHorizontalHeaderLabels(header);
    ui->tableWidget->horizontalHeader()->setStretchLastSection(true);
    ui->tableWidget->horizontalHeader()->setHighlightSections(false);
    ui->tableWidget->setStyleSheet("selection-background-color:#9ceaea");
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableWidget->setSelectionMode(QTableWidget::SingleSelection);
}

void device_manager ::selectall(){
    disconnect(ui->tableWidget, &QTableWidget::itemChanged, this, &device_manager::onItemChanged);
    QSqlQuery query("select * from  device");
    query.exec();
    storge(query);
    connect(ui->tableWidget, &QTableWidget::itemChanged, this, &device_manager::onItemChanged);
}


void device_manager::setSliderStyle(QSlider* slider) {
    QString styleSheet = R"(
        QSlider::groove:horizontal {
            border: 0px solid #bbb;
        }
        QSlider::sub-page:horizontal {
            background: rgb(90,49,255);
            border-radius: 2px;
            margin-top:13px;
            margin-bottom:13px;
        }
        QSlider::add-page:horizontal {
            background: rgb(255,255, 255);
            border: 0px solid #777;
            border-radius: 2px;
            margin-top:9px;
            margin-bottom:9px;
        }
        QSlider::handle:horizontal {
            background: rgb(193,204,208);
            width:14px;
            border: 1px solid rgb(193,204,208);
            border-radius: 7px;
            margin-top:9px;
            margin-bottom:9px;
        }
        QSlider::handle:horizontal:hover {
            background: rgb(193,204,208);
            width: 20px;
            border: 1px solid rgb(193,204,208);
            border-radius: 10px;
            margin-top:6px;
            margin-bottom:6px;
        }
    )";
    slider->setStyleSheet(styleSheet);
}


void device_manager::storge(QSqlQuery query)
{
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
        int     devStatus = query.value(2).toInt();
        int     volume    = query.value(3).toInt();
        int     groupNo   = query.value(4).toInt();
        double  longitude = query.value(5).toDouble();
        double  latitude  = query.value(6).toDouble();



        ui->tableWidget->setItem(i,0,new QTableWidgetItem(QString::number(devNo)));
        ui->tableWidget->setItem(i,1,new QTableWidgetItem(devName));
        ui->tableWidget->setItem(i,2,new QTableWidgetItem(QString::number(devStatus)));

        MySlider *slider = new MySlider;
        setSliderStyle(slider);
        slider->setRange(0, 100);  // 假设volume的范围是0到100
        slider->setValue(volume);
        QWidget w;
        ui->tableWidget->setCellWidget(i, 3, slider);
        slider->setProperty("devNo", devNo);  // 存储设备的 ID

        // 连接 sliderReleased 信号到 updateDatabase 槽
        connect(slider, &QSlider::sliderReleased, this, &device_manager::updateVolume);
        ui->tableWidget->setCellWidget(i, 3, slider);


        ui->tableWidget->setItem(i,4,new QTableWidgetItem(QString::number(groupNo)));
        ui->tableWidget->setItem(i,5,new QTableWidgetItem(QString::number(longitude, 'f', 8)));
        ui->tableWidget->setItem(i,6,new QTableWidgetItem(QString::number(latitude, 'f', 8)));

        i++;
    }
    // ui->tableWidget->selectRow(index);
}



void device_manager::updateVolume()
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


void device_manager::on_addDevBtn_clicked()
{
    addUpdateDev *adddev = new addUpdateDev();
    adddev->setWindowTitle("增加设备");
    adddev->show();
    // connect(adddev, &addUpdateDev::addDevData, this, &device_manager::addDev);
    connect(adddev ,SIGNAL(addDevData(dev_Data)),this,SLOT(addDev(dev_Data)));

}

void device_manager::addDev(dev_Data devdata){
    QSqlQuery query("INSERT INTO device (devNo , devName, devStatus, volume, groupNo, longitude, latitude,rentStatus ) VALUES ( ?, ?, ?, ?, ?,?,?,0)");
    query.addBindValue(devdata.devNo);
    query.addBindValue(devdata.devName);
    query.addBindValue(devdata.devStatus);
    query.addBindValue(devdata.volume);
    query.addBindValue(devdata.groupNo);
    query.addBindValue(devdata.longitude);
    query.addBindValue(devdata.latitude);

    if(query.exec()){
        qDebug()<<"创建成功";
    }
    else{
        qDebug()<<query.lastError();
    }
    selectall();

}

void device_manager::on_pushButton_4_clicked()
{
    disconnect(ui->tableWidget, &QTableWidget::itemChanged, this, &device_manager::onItemChanged);
    QSqlQuery query;
    QString string = "select * from device where 1";
    query.prepare(string);
    if(!ui->searchLineEdit->text().isEmpty()){
        int devNo = ui->searchLineEdit->text().toInt();
        query.prepare(string+" and devNo = :devNo;");
        query.bindValue(":devNo",devNo);

        qDebug()<<query.lastError().text();

    }

    query.exec();
    storge(query);
    connect(ui->tableWidget, &QTableWidget::itemChanged, this, &device_manager::onItemChanged);
}


void device_manager::on_updateDevBtn_clicked()
{
    disconnect(ui->tableWidget, &QTableWidget::itemChanged, this, &device_manager::onItemChanged);
    for (int i = 0; i < ui->tableWidget->rowCount(); ++i)
    {
        bool isRowModified = false;  // 引入的变量，用于记录该行是否被修改

        int devNo = ui->tableWidget->item(i, 0)->text().toInt();
        QString devName = ui->tableWidget->item(i, 1)->text();
        int devStatus = ui->tableWidget->item(i, 2)->text().toInt();
        int groupNo = ui->tableWidget->item(i, 4)->text().toInt();
        double longitude = ui->tableWidget->item(i, 5)->text().toDouble();
        double latitude = ui->tableWidget->item(i, 6)->text().toDouble();

        for (int j = 0; j < ui->tableWidget->columnCount(); ++j)
        {
            if (j != 3)  // 对于滑块，我们需要单独处理
            {
                if (ui->tableWidget->item(i, j)->backgroundColor() == Qt::red)
                {

                    ui->tableWidget->item(i, j)->setBackgroundColor(Qt::white);
                    isRowModified = true;

                }
            }
        }

        if (isRowModified)
        {
            QSqlQuery query;
            query.prepare("UPDATE device SET devName = :devName, devStatus = :devStatus, groupNo = :groupNo, longitude = :longitude, latitude = :latitude WHERE devNo = :devNo");
            query.bindValue(":devNo", devNo);
            query.bindValue(":devName", devName);
            query.bindValue(":devStatus", devStatus);
            query.bindValue(":groupNo", groupNo);
            query.bindValue(":longitude", longitude);
            query.bindValue(":latitude", latitude);
            query.exec();
        }


    }
    connect(ui->tableWidget, &QTableWidget::itemChanged, this, &device_manager::onItemChanged);
}


void device_manager::on_pushButton_clicked()
{

    selectall();

}


void device_manager::on_deleteDevBtn_clicked()
{
    int currentRow = ui->tableWidget->currentRow();
    if (currentRow != -1)  // 如果有选中的行
    {
        QString devNo = ui->tableWidget->item(currentRow, 0)->text();
        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(this, "删除设备", "是否删除设备编号为 " + devNo + " 的设备？",
                                      QMessageBox::Yes|QMessageBox::No);
        if (reply == QMessageBox::Yes)
        {
            QSqlQuery query;
            query.prepare("DELETE FROM device WHERE devNo = :devNo");
            query.bindValue(":devNo", devNo);
            query.exec();
            // 从表格中移除该行
            ui->tableWidget->removeRow(currentRow);
        }
    }
}

void device_manager::onItemChanged(QTableWidgetItem* item)
{
    item->setBackgroundColor(Qt::red);
    qDebug()<<"11";
}

