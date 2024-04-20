#include "group_manager.h"
#include "UI/device/device_manager.h"
#include "UI/device/mytreewidget.h"
#include "function/dbinteraction.h"
#include "function/device.h"
#include "qdebug.h"
#include "qheaderview.h"
#include "qsqlquery.h"
#include "ui_group_manager.h"

group_manager::group_manager(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::group_manager)
{
    ui->setupUi(this);
    myTreeWidget = new MyTreeWidget;

    // 设置MyTreeWidget的属性
    myTreeWidget->setDragEnabled(true);
    myTreeWidget->setAcceptDrops(true);
    myTreeWidget->setDragDropMode(QAbstractItemView::InternalMove);

    // 将MyTreeWidget添加到你的页面中
    ui->widget->setLayout(new QVBoxLayout);
    ui->widget->layout()->addWidget(myTreeWidget);
    ui->spinBox->setMaximum(10000);

    connect(myTreeWidget, &MyTreeWidget::itemMoved, this, &group_manager::onItemMoved);
    connect(myTreeWidget, &QTreeWidget::itemClicked, this, &group_manager::onItemClicked);
    updateTree();

}

group_manager::~group_manager()
{
    delete ui;
}


void group_manager::onItemMoved(QString deviceName, QString groupName){
    uint devNo = getDevNo(deviceName);
    uint groupNo = getGroupNo(groupName);
    // 移动设备到新的分组
    DeleteDevToGroup(devNo);
    InsertDevToGroup(devNo, groupNo);
    updateTree();


}

void group_manager::updateTree() {
    myTreeWidget->header()->setHidden(true);
    myTreeWidget->clear();  // 清空旧的数据
    auto groups = GetAllGroup();
    for (const auto& group : groups) {
        QTreeWidgetItem* groupItem = new QTreeWidgetItem(myTreeWidget);
        groupItem->setText(0, group.groupName);

        auto devices = SelectGpDevs(group.groupNo);
        for (const auto& device : devices) {
            QTreeWidgetItem* deviceItem = new QTreeWidgetItem(groupItem);
            deviceItem->setText(0, device.devName);
        }
    }

    myTreeWidget->expandAll();  // 展开所有节点
}

void group_manager::onItemClicked(QTreeWidgetItem *item, int column)
{
    QString device = item->text(0);
    QSqlQuery query;
    query.prepare("select * from device where dev_name = :device");
    query.bindValue(":device",device);
    query.exec();
    query.next();




    ui->nameline->setText(query.value(1).toString());
    ui->spinBox->setValue(query.value(0).toInt());

    ui->lineEdit->setText(query.value(3).toString());
    ui->lineEdit_2->setText(query.value(4).toString());

    MySlider *slider = new MySlider(ui->volume);
    device_manager d;
    d.setSliderStyle(slider);
    slider->setRange(0, 100);  // 假设volume的范围是0到100
    slider->setValue(query.value(2).toInt());


    slider->setProperty("devNo", query.value(0).toInt());  // 存储设备的 ID

    // 连接 sliderReleased 信号到 updateDatabase 槽
    connect(slider, &QSlider::valueChanged, [=](int volume){
        DeviceModule::getInstance()->setVolume(query.value(0).toInt(),volume);
    });




}
