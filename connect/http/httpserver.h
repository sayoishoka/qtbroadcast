#ifndef HTTPSERVER_H
#define HTTPSERVER_H

#include <QObject>
#include <QTcpServer>
#include<QTcpSocket>
#include<connect/http/httpsession.h>
class httpserver : public QTcpServer
{
    Q_OBJECT

private:
    static httpserver *servers ;



protected:
    void incomingConnection(qintptr socketDescriptor) override
    {
        QTcpSocket *socket = new QTcpSocket(this);
        socket->setSocketDescriptor(socketDescriptor);

        // 创建一个新的HttpSession对象来处理这个连接
        new httpSession(socket, this);
    }

public:
    explicit httpserver(QObject *parent = nullptr);

    void  Listen(int port){
        if (!this->listen(QHostAddress::Any, port)) {
            qDebug() << "Could not start server";
        } else {
            qDebug() << "Listening to port " << port << "...";
        }
    }

    static httpserver* getserver(){

        return servers;
    }

};

#endif // HTTPSERVER_H
