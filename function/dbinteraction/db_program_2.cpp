#include "db_program_2.h"

db_program_2::db_program_2(QObject *parent) : QObject(parent)
{
    Dispatcher::getDispatcher()->Register("allHadPrograms",std::bind(&db_program_2::allHadPrograms, this,std::placeholders::_1));
    Dispatcher::getDispatcher()->Register("pmOnplay",std::bind(&db_program_2::pmOnplay, this,std::placeholders::_1));
    Dispatcher::getDispatcher()->Register("searchSinger",std::bind(&db_program_2::searchSinger, this,std::placeholders::_1));
    Dispatcher::getDispatcher()->Register("singerSound",std::bind(&db_program_2::singerSound, this,std::placeholders::_1));
    Dispatcher::getDispatcher()->Register("onClickPm",std::bind(&db_program_2::onClickPm, this,std::placeholders::_1));
    Dispatcher::getDispatcher()->Register("getClickPm",std::bind(&db_program_2::getClickPm, this,std::placeholders::_1));
}

QJsonObject db_program_2::allHadPrograms(QJsonObject &obj)
{
    QSqlQuery query;
    QJsonArray pmlist;
    query.exec("SELECT * FROM program");
    while(query.next()){
        QJsonObject pm;
        pm.insert("pmId",query.value(0).toInt());
        pm.insert("playDuration",query.value(4).toString());
        pm.insert("pmName",query.value(1).toString());
        pm.insert("startDate",query.value(5).toString());
        pm.insert("endDate",query.value(6).toString());
        pm.insert("startTime",query.value(9).toString());
        pm.insert("pmTyp",query.value(2).toString());
        pm.insert("pmStatue",QVariant(query.value(7)).toBool());
        pm.insert("weekCnt",query.value(8).toInt());
        pmlist.append(pm);
    }
    return QJsonObject{
     {"code", 200},
     {"message", "成功"},
     {"data", pmlist}
    };
}

QJsonObject db_program_2::pmOnplay(QJsonObject &obj)
{
    QString pmStatue = (obj.value("pmStatue").toBool() ? "1" : "0");
    QSqlQuery query;
    QJsonArray pmlist;
    query.exec("SELECT * FROM program WHERE pm_statue = " + pmStatue);
    while(query.next()){
        QJsonObject pm;
        pm.insert("pmId",query.value(0).toInt());
        pm.insert("playDuration",query.value(4).toString());
        pm.insert("pmName",query.value(1).toString());
        pm.insert("startDate",query.value(5).toString());
        pm.insert("endDate",query.value(6).toString());
        pm.insert("startTime",query.value(9).toString());
        pm.insert("pmTyp",query.value(2).toString());
        pm.insert("pmStatue",QVariant(query.value(7)).toBool());
        pm.insert("weekCnt",query.value(8).toInt());
        pmlist.append(pm);
    }
    return QJsonObject{
     {"code", 200},
     {"message", "成功"},
     {"data", pmlist}
    };
}

QJsonObject db_program_2::searchSinger(QJsonObject &obj)
{
    QSqlQuery query;
    QJsonArray singerlist;
    query.exec("SELECT * FROM singer");
    while(query.next()){
        QJsonObject singer;
        singer.insert("singerId",query.value(0).toInt());
        singer.insert("singer_Name",query.value(1).toString());
        singer.insert("singer_Picture",query.value(2).toString());
        singerlist.append(singer);
    }
    return QJsonObject{
     {"code", 200},
     {"message", "成功"},
     {"data", singerlist}
    };
}

QJsonObject db_program_2::singerSound(QJsonObject &obj)
{
    QString singerId = QString::number(obj.value("singerId").toInt());
    QSqlQuery query;
    QJsonArray soundlist;
    query.exec("SELECT file_no,file_name,sound_picture FROM FILE WHERE singer_no = "+singerId);
    while(query.next()){
        QJsonObject sound;
        sound.insert("soundId",query.value(0).toInt());
        sound.insert("soundName",query.value(1).toString());
        sound.insert("sound_Picture",query.value(2).toString());
        soundlist.append(sound);
    }
    return QJsonObject{
     {"code", 200},
     {"message", "成功"},
     {"data", soundlist}
    };
}

QJsonObject db_program_2::onClickPm(QJsonObject &obj)
{
    QJsonArray singId = obj.value("sing_Ids").toArray();
    QString singids = "";
    for (int i = 0; i < singId.size(); ++i) {
        int value = singId.at(i).toInt();
        if (i == 0){
            singids += value;
        }else{
            singids += ","+value;
        }
    }
    QSqlQuery query;
    QJsonArray soundlist;
    query.exec("SELECT file_no,file_Duration,sound_picture,file_name,file_type FROM FILE WHERE file_no IN ("+singids+")");
    while(query.next()){
        QJsonObject sound;
        sound.insert("sing_Id",query.value(0).toInt());
        sound.insert("sing_Duration",query.value(1).toString());
        sound.insert("sing_Picture",query.value(2).toString());
        sound.insert("sing_Name",query.value(3).toString());
        sound.insert("playType",query.value(4).toString());
        soundlist.append(sound);
    }
    return QJsonObject{
     {"code", 200},
     {"message", "成功"},
     {"data", soundlist}
    };
}

QJsonObject db_program_2::getClickPm(QJsonObject &obj)
{
    QSqlQuery query;
    QJsonArray soundlist;
    query.exec("SELECT file_no,file_Duration,sound_picture,file_name,file_type FROM FILE");
    while(query.next()){
        QJsonObject sound;
        sound.insert("singId",query.value(0).toInt());
        sound.insert("sing_Duration",query.value(1).toString());
        sound.insert("sing_Picture",query.value(2).toString());
        sound.insert("sing_Name",query.value(3).toString());
        sound.insert("playType",query.value(4).toString());
        soundlist.append(sound);
    }
    return QJsonObject{
     {"code", 200},
     {"message", "成功"},
     {"data", soundlist}
    };
}
