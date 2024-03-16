#ifndef MYDATA_H
#define MYDATA_H
#include "qdatetime.h"
#include<QString>

typedef struct devData{
    int     devNo     ;
    QString devName   ;
    int     devStatus ;
    int     volume    ;
    int     groupNo   ;
    double  longitude ;
    double  latitude  ;
}dev_Data;

typedef struct billingData{
    int oId;
    int userId;
    int cost;
    QDateTime begingTime;
    int rentalTime;
    int usetime;
    int ostatus;
    QDateTime endTime;
}billing_Data;

#endif // MYDATA_H
