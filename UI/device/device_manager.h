#ifndef DEVICE_MANAGER_H
#define DEVICE_MANAGER_H

#include "qsqlquery.h"
#include <QWidget>
#include<QSlider>
#include"UI/MySlider.h"

#include<UI/Mydata.h>
#include"addupdatedev.h"
#include "qtablewidget.h"
namespace Ui {
class device_manager;
}

class device_manager : public QWidget
{
    Q_OBJECT

public:
    explicit device_manager(QWidget *parent = nullptr);
    ~device_manager();

    void setSliderStyle(QSlider *slider);

private slots:
    void on_addDevBtn_clicked();
    void addDev(dev_Data devdata);

    void on_pushButton_4_clicked();

    void on_updateDevBtn_clicked();

    void on_pushButton_clicked();

    void on_deleteDevBtn_clicked();

private:
    Ui::device_manager *ui;
    void setcontrol();
    void storge(QSqlQuery query);
    void selectall();
    void Connectdatabase();
    void updateVolume();



    void onItemChanged(QTableWidgetItem *item);
};

#endif // DEVICE_MANAGER_H
