#ifndef HTTPSESSION_H
#define HTTPSESSION_H

#include "qtcpsocket.h"
#include <QObject>
#include<QTcpSocket>
class httpSession : public QObject
{
    Q_OBJECT
public:
    explicit httpSession(QTcpSocket *socket,QObject *parent = nullptr);

private:
    QTcpSocket *mysocket;
    void handleHttpRequest();
    void socketDisconnected();
signals:
};

#endif // HTTPSESSION_H
