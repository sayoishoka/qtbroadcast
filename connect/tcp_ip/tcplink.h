#ifndef TCPLINK_H
#define TCPLINK_H

#include "broadcastmain.h"
#include "connect/dispatcher.h"
#include <functional>
#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <QJsonDocument>
#include <QJsonParseError>
#include <QJsonArray>
#include <QJsonObject>
#include <QDebug>
#include <QDataStream>

class TcpLink : public QObject
{
    Q_OBJECT
public:
    static TcpLink* getTcpLink(){
        static TcpLink instance;
        return &instance;
    };
    QJsonObject UserLogin(QJsonObject &Obj);
    QJsonObject getAllFuncs(QJsonObject &obj);
    void WriteData(QJsonObject &obj);
signals:
    void change_state_yes();
    void change_state_no();
private:
    void JsonAnalysis(QByteArray &buf);

private slots:
    void newConnectSlot();
    void readyRead_Slot();
    void Disconnected_Slot();
private:
    QTcpServer *tcpserver;
    QTcpSocket *socket;
    explicit TcpLink(QObject *parent = nullptr);
};


#endif // TCPLINK_H
