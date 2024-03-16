#include "rights_management.h"
#include "function/dbinteraction/dbselectupdate.h"
#include "ui_rights_management.h"

Rights_management::Rights_management(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Rights_management)
{
    ui->setupUi(this);
    Dispatcher::getDispatcher()->Register("getRoleFunc",std::bind(&Rights_management::getRoleFunc, this,std::placeholders::_1));
    Dispatcher::getDispatcher()->Register("addPms",std::bind(&Rights_management::addPms, this,std::placeholders::_1));
    Dispatcher::getDispatcher()->Register("removePms",std::bind(&Rights_management::removePms, this,std::placeholders::_1));
}

Rights_management::~Rights_management()
{
    delete ui;
}

QJsonObject Rights_management::getRoleFunc(QJsonObject &obj)
{
    QString roleno = QString::number(obj.value("roleNo").toInt());
    dbSelectUpdate s;
    QSqlQuery query = s.getData_Sheet("SELECT function.func_no,function.func_name FROM role "
                                                   "LEFT JOIN permission ON role.role_no=permission.role_no "
                                                   "LEFT JOIN FUNCTION ON permission.func_no=function.func_no "
                                                   "WHERE role.role_no = '"+ roleno +"'");
    QJsonObject requestjson;
    QJsonObject funNo;
    QJsonArray funcList;
    QJsonArray childNoList;
    requestjson.insert("response","reGetRoleFunc");
    int flag=0;
    while(query.next()){
//        qDebug()<<query.value(0)<<query.value(2);
        if (query.value(0).toInt()==1000||query.value(0).toInt()==2000||query.value(0).toInt()==3000||
                query.value(0).toInt()==4000||query.value(0).toInt()==5000){
            if (flag!=0){
                funNo.insert("childNoList",childNoList);
                childNoList = QJsonArray();
                funcList.append(funNo);
                // 清空对象中的内容
                funNo = QJsonObject();
            }
            funNo.insert("funcNo",query.value(0).toInt());
            funNo.insert("funcName",query.value(1).toString());
            flag++;
        }else{
            QJsonObject child;
            child.insert("funcNo",query.value(0).toInt());
            child.insert("funcName",query.value(1).toString());
            childNoList.append(child);
        }
    }
    requestjson.insert("funcList",funcList);
    return requestjson;
}

QJsonObject Rights_management::addPms(QJsonObject &obj)
{
    QString roleNo = QString::number(obj.value("roleNo").toInt());
    QString funcNo = QString::number(obj.value("funcNo").toInt());
    QString faNo = QString::number(obj.value("faNo").toInt());
    QString func;
    dbSelectUpdate s;
    QSqlQuery query = s.getData_Sheet("SELECT func_no FROM FUNCTION WHERE func_no = '"+funcNo+"' AND fa_no = '"+faNo+"'");
    while(query.next()){
        func = query.value(0).toString();
    }
    QSqlQuery query2 = s.getData_Sheet("INSERT INTO permission (role_no,func_no) VALUES ('"+roleNo+"','"+func+"')");
    QJsonObject requestjson;
    requestjson.insert("response","reAddPms");
    requestjson.insert("status",true);
    return requestjson;
}

QJsonObject Rights_management::removePms(QJsonObject &obj)
{
    QString roleNo = QString::number(obj.value("roleNo").toInt());
    QString funcNo = QString::number(obj.value("funcNo").toInt());
    QString faNo = QString::number(obj.value("faNo").toInt());
    QString func;
    dbSelectUpdate s;
    QSqlQuery query = s.getData_Sheet("SELECT func_no FROM FUNCTION WHERE func_no = '"+funcNo+"' AND fa_no = '"+faNo+"'");
    while(query.next()){
        func = query.value(0).toString();
    }
    QSqlQuery query2 = s.getData_Sheet("DELETE FROM permission WHERE role_no='"+roleNo+"' AND func_no='"+func+"'");
    QJsonObject requestjson;
    requestjson.insert("response","reRemovePms");
    requestjson.insert("status",true);
    return requestjson;
}
