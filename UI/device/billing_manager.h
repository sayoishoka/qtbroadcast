#ifndef BILLING_MANAGER_H
#define BILLING_MANAGER_H

#include "qsqlquery.h"
#include "qtablewidget.h"
#include <QWidget>
#include"billingdetail.h"
namespace Ui {
class billing_manager;
}

class billing_manager : public QWidget
{
    Q_OBJECT

public:
    explicit billing_manager(QWidget *parent = nullptr);
    ~billing_manager();

public slots:
    void endbilling(int oId, QDateTime endtime);

private slots:
    void query_Detail_INfor(QTableWidgetItem *item);

private:
    Ui::billing_manager *ui;
    void prepare();
    void selectall();
    void showdata(QSqlQuery query);
    void updateTime();


    void updatedatabase();
    void checkAndUpdateRecords();

};

#endif // BILLING_MANAGER_H
