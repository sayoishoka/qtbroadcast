#ifndef BILLINGDETAIL_H
#define BILLINGDETAIL_H

#include "UI/Mydata.h"
#include <QWidget>
#include<QSqlQuery>
namespace Ui {
class billingdetail;
}

class billingdetail : public QWidget
{
    Q_OBJECT

public:
    explicit billingdetail(billing_Data billing,QSqlQuery query);
    ~billingdetail();

signals:
    void stopbilling(int oId,QDateTime currenttime);

private slots:


    void on_stopbtn_clicked();

private:
    Ui::billingdetail *ui;
    void prepare();
    void prepareongoing();
    void preparecomplain();
    void showbilling(billing_Data billing);
    void showdev(QSqlQuery query);
    void updateVolume();
    void updateUsetime();

    QTimer *timer;
};

#endif // BILLINGDETAIL_H
