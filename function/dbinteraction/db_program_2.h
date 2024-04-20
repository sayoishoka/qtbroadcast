#ifndef DB_PROGRAM_2_H
#define DB_PROGRAM_2_H

#include <QObject>
#include <QSqlQuery>
#include <QJsonObject>
#include <QJsonArray>
#include "connect/dispatcher.h"
#include "function/dbinteraction/dbselectupdate.h"

class db_program_2 : public QObject
{
    Q_OBJECT
public:
    static db_program_2* getdb_program_2(){
        static db_program_2 instance;
        return &instance;
    };


signals:

private:
    explicit db_program_2(QObject *parent = nullptr);
    QJsonObject allHadPrograms(QJsonObject &obj);
    QJsonObject pmOnplay(QJsonObject &obj);
    QJsonObject searchSinger(QJsonObject &obj);
    QJsonObject singerSound(QJsonObject &obj);
    QJsonObject onClickPm(QJsonObject &obj);
    QJsonObject getClickPm(QJsonObject &obj);
};

#endif // DB_PROGRAM_2_H
