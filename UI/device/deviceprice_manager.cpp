#include "deviceprice_manager.h"
#include "qdebug.h"
#include "qmessagebox.h"
#include "ui_deviceprice_manager.h"
#include<QCheckBox>
deviceprice_manager::deviceprice_manager(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::deviceprice_manager)
{
    ui->setupUi(this);

    prepare();
    selectall();
}


deviceprice_manager::~deviceprice_manager()
{
    delete ui;
}

void deviceprice_manager::prepare(){

    ui->tableWidget->setColumnCount(6);
    QStringList header;
    header<<"设备编号"<<"设备名称"<<"每分钟价格"<<"每小时价格"<<"每天价格"<<"需要修改价格设备";

    ui->tableWidget->setHorizontalHeaderLabels(header);
    ui->tableWidget->horizontalHeader()->setHighlightSections(false);
    ui->tableWidget->setStyleSheet("selection-background-color:#9ceaea");
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableWidget->horizontalHeader()->setStretchLastSection(false);
    ui->tableWidget->setSelectionMode(QTableWidget::SingleSelection);
    ui->tableWidget->setSelectionMode(QAbstractItemView::NoSelection);
}

void deviceprice_manager ::selectall(){
    QSqlQuery query("select devNo,devName,minute,hour,day from  device");
    query.exec();
    showdata(query);
}


void deviceprice_manager::showdata(QSqlQuery query){
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
        int     minute    = query.value(2).toInt();
        int     hour      = query.value(3).toInt();
        int     day       = query.value(4).toInt();




        ui->tableWidget->setItem(i,0,new QTableWidgetItem(QString::number(devNo)));
        ui->tableWidget->setItem(i,1,new QTableWidgetItem(devName));
        ui->tableWidget->setItem(i,2,new QTableWidgetItem(QString::number(minute)));
        ui->tableWidget->setItem(i,3,new QTableWidgetItem(QString::number(hour)));
        ui->tableWidget->setItem(i,4,new QTableWidgetItem(QString::number(day)));

        QCheckBox *checkbox = new QCheckBox;
        ui->tableWidget->setCellWidget(i, 5, checkbox);

        i++;
    }
}

void deviceprice_manager::on_pushButton_clicked()
{
    int newPrice = ui->newPriceLineEdit->text().toInt();
    if (newPrice <= 0) {
        QMessageBox::warning(this, "警告", "请输入一个有效的价格");
        return;
    }

    // 获取用户选择的价格类型
    int priceType = ui->comboBox->currentIndex();
    QString columnName;
    if (priceType == 0) {
        columnName = "minute";
    } else if (priceType == 1) {
        columnName = "hour";
    } else if (priceType == 2) {
        columnName = "day";
    } else {
        QMessageBox::warning(this, "警告", "请选择一个价格类型");
        return;
    }

    // 遍历表格中的所有行
    for (int i = 0; i < ui->tableWidget->rowCount(); ++i) {
        QCheckBox* checkbox = qobject_cast<QCheckBox*>(ui->tableWidget->cellWidget(i, 5));
        if (checkbox && checkbox->isChecked()) {
            // 获取设备编号
            int devNo = ui->tableWidget->item(i, 0)->text().toInt();

            // 更新数据库
            QSqlQuery query;
            query.prepare(QString("UPDATE device SET %1 = :newPrice WHERE devNo = :devNo").arg(columnName));
            query.bindValue(":newPrice", newPrice);
            query.bindValue(":devNo", devNo);
            if (!query.exec()) {
                QMessageBox::warning(this, "警告", "更新价格失败");
                return;
            }

        }
    }

    selectall();
    qDebug()<<"2";
}

