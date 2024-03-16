#include "tcplink.h"
#include "function/dbinteraction/dbselectupdate.h"

TcpLink::TcpLink(QObject *parent) : QObject(parent)
{
    tcpserver = new QTcpServer(this);
    connect(tcpserver,SIGNAL(newConnection()),this,SLOT(newConnectSlot()));
    tcpserver->listen(QHostAddress::Any,8899);
    socket=new QTcpSocket(this);
    Dispatcher *dispatch = Dispatcher::getDispatcher();
    dispatch->Register("login",std::bind(&TcpLink::UserLogin, this,std::placeholders::_1));
    dispatch->Register("getAllFuncs",std::bind(&TcpLink::getAllFuncs, this,std::placeholders::_1));
}

void TcpLink::newConnectSlot()
{
    socket=tcpserver->nextPendingConnection();//获取已经连接的客户端套接字
    qDebug() << "newConnectSlot执行";
    emit change_state_yes();
    connect(socket,SIGNAL(readyRead()),this,SLOT(readyRead_Slot()));
    connect(socket,SIGNAL(disconnected()),this,SLOT(Disconnected_Slot()));
}
///
/// \brief TcpLink::readyRead_Slot
///接受客户端请求处理
void TcpLink::readyRead_Slot()
{
    QByteArray array = socket->readAll();
    JsonAnalysis(array);
    qDebug()<<array;
}

void TcpLink::Disconnected_Slot()
{
    emit change_state_no();
}

void TcpLink::JsonAnalysis(QByteArray &buf)
{
//    QDataStream out();
//    out.setVersion(QDataStream::Qt_4_1);
//    QDataStream in(socket);
//    while(!in.atEnd()) {
//        QByteArray array;
//        qint64 size;
//        in >> size;
//        in >> array;
//        QJsonDocument doc = QJsonDocument::fromJson(array);
//        QJsonObject rootObj = doc.object();
//        QJsonObject writejs = Dispatcher::getDispatcher()->Dispatch(rootObj.value("request").toString(),rootObj);
//        WriteData(writejs);
//    }
    QJsonParseError err;
    QJsonDocument doc = QJsonDocument::fromJson(buf, &err);
    if (err.error != QJsonParseError::NoError){
        qDebug()<<"1";
        return;
    }
    QJsonObject rootObj = doc.object();
    QJsonObject writejs = Dispatcher::getDispatcher()->Dispatch(rootObj.value("request").toString(),rootObj);
    WriteData(writejs);
//    if (QString::localeAwareCompare(rootObj.value("request").toString(),"login")==0){
//        UserLogin(rootObj);
//    }else if (QString::localeAwareCompare(rootObj.value("request").toString(),"getAllFuncs")==0){
//        getAllFuncs(rootObj);
//    }
}

QJsonObject TcpLink::UserLogin(QJsonObject &Obj)
{
    QString username = Obj.value("userName").toString();
    QString userpsd = Obj.value("userPw").toString();
    QJsonObject requestjson;
    QJsonDocument requestdoc;
    QByteArray requestarray;
    dbSelectUpdate s;
    QSqlQuery query = s.getData_Sheet("select * from user where user_name='" + username +"'");
    if (query.size()==-1||query.size()==0){
        requestjson.insert("response", "reLogin");
        requestjson.insert("status", false);
        requestjson.insert("msg", "用户名有误");
        return requestjson;
//        requestdoc.setObject(requestjson);
//        requestarray = requestdoc.toJson();
//        socket->write(requestarray);
    }else{
        while(query.next()){
            qDebug()/*<<query.value("id").toInt()*/
                     <<query.value(0).toInt()
                    <<query.value(1).toString().toUtf8().data()
                   <<query.value(2).toString().toUtf8().data()
                  <<query.value(3).toString().toUtf8().data()
                 <<query.value(4).toString().toUtf8().data()
                <<query.value(5).toInt();
            if (QString::localeAwareCompare(userpsd,query.value("user_PW").toString())!=0){
                requestjson.insert("response", "reLogin");
                requestjson.insert("status", false);
                requestjson.insert("msg", "密码有误");
                return requestjson;
//                requestdoc.setObject(requestjson);
//                requestarray = requestdoc.toJson();
//                socket->write(requestarray);
            }else{
                QJsonArray funcList;
                requestjson.insert("response", "reLogin");
                requestjson.insert("status", true);
                requestjson.insert("msg", "登录成功");
                QJsonObject funNo;
                QJsonArray childNoList;
                QSqlQuery query2 = s.getData_Sheet("SELECT function.func_no FROM USER "
                                                                "LEFT JOIN role ON user.role_no=role.role_no "
                                                                "LEFT JOIN permission ON role.role_no=permission.role_no "
                                                                "LEFT JOIN FUNCTION ON permission.func_no=function.func_no "
                                                                "WHERE user.user_name = '"+username+"'");

                int flag=0;
                while(query2.next()){
                    qDebug()<<query2.value(0);
                    if (query2.value(0).toInt()==1000||query2.value(0).toInt()==2000||query2.value(0).toInt()==3000||
                            query2.value(0).toInt()==4000||query2.value(0).toInt()==5000){
                        if (flag!=0){
                            funNo.insert("childNoList",childNoList);
                            childNoList = QJsonArray();
                            funcList.append(funNo);
                            // 清空对象中的内容
                            funNo = QJsonObject();
                        }
                        funNo.insert("funcNo",query2.value(0).toInt());
                        flag++;
                    }else{
                        childNoList.append(query2.value(0).toInt());
                    }
                }
                requestjson.insert("funcList", funcList);
            }
        }
    }
    qDebug()<<requestjson;
//    requestdoc.setObject(requestjson);
//    requestarray = requestdoc.toJson();
    return requestjson;
}

QJsonObject TcpLink::getAllFuncs(QJsonObject &obj)
{
    QJsonObject requestjson;
    QJsonDocument requestdoc;
    QByteArray requestarray;
    QJsonObject funNo;
    QJsonArray funcList;
    QJsonArray childNoList;
    requestjson.insert("response","reGetAllFuncs");
    dbSelectUpdate s;
    QSqlQuery query = s.getData_Sheet("SELECT * FROM FUNCTION");
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
            funNo.insert("funcName",query.value(2).toString());
            flag++;
        }else{
            QJsonObject child;
            child.insert("funcNo",query.value(0).toInt());
            child.insert("funcName",query.value(2).toString());
            childNoList.append(child);
        }
    }
    requestjson.insert("funcList",funcList);
//    requestdoc.setObject(requestjson);
//    requestarray = requestdoc.toJson();
//    socket->write(requestarray);
    qDebug()<<"获取所有功能";
    qDebug()<<requestjson;
    return requestjson;
}

void TcpLink::WriteData(QJsonObject &obj)
{
    QJsonDocument doc;
    doc.setObject(obj);
    QByteArray array = doc.toJson();
    qDebug()<<QString(array);
    qDebug()<<array.size();
    QDataStream out(socket);
    out.setVersion(QDataStream::Qt_4_1);
    out << (qint64)array.size();
//    int res = out.writeRawData(array.constData(), array.size());
//    qDebug() << strlen(array.constData());
    out << array;
    socket->flush();
}
