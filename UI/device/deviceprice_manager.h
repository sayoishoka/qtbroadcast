#ifndef DEVICEPRICE_MANAGER_H
#define DEVICEPRICE_MANAGER_H

#include "qsqlquery.h"
#include <QWidget>

namespace Ui {
class deviceprice_manager;
}

class deviceprice_manager : public QWidget
{
    Q_OBJECT

public:
    explicit deviceprice_manager(QWidget *parent = nullptr);
    ~deviceprice_manager();


private slots:
    void on_pushButton_clicked();

private:
    Ui::deviceprice_manager *ui;
    void prepare();
    void selectall();
    void showdata(QSqlQuery query);
};

#endif // DEVICEPRICE_MANAGER_H
